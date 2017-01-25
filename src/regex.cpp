/**
 * @file	regex.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "regex.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Constants -- */

namespace
{
  // infix operators
  const char UNION_OP = '|';
  const char CONCAT_OP = '.';

  // postfix operators
  const char OPTIONAL_OP = '?';
  const char KLEENE_OP = '*';
  const char REPEAT_OP = '+';
}

/* -- Private Procedures -- */

namespace
{

  /** Returns `true` if the specified operator is an infix operator. */
  bool is_infix_operator(char op)
  {
    switch (op)
    {
    case UNION_OP:
    case CONCAT_OP:
      return true;
    default:
      return false;
    }
  }

  /** Returns `true` if the specified operator is a postfix operator. */
  bool is_postfix_operator(char op)
  {
    switch (op)
    {
    case OPTIONAL_OP:
    case KLEENE_OP:
    case REPEAT_OP:
      return true;
    default:
      return false;
    }
  }

  /** Returns `true` if the specified infix operator is left associative. */
  bool is_left_assoc_operator(char ch)
  {
    // this doesn't strictly matter since both of these operators are left/right-associative,
    // but I have to make this distinction in order to get the same output as the reference
    // implementation.
    switch (ch)
    {
    case UNION_OP:
      return false;
    case CONCAT_OP:
      return true;
    default:
      abort(); // logic error!
    }
  }

  /** Returns `true` if the specified character is an operator. */
  bool is_operator(char ch)
  {
    return (is_infix_operator(ch) || is_postfix_operator(ch));
  }

  /** Returns the precedence for the specified operator. */
  int operator_precedence(char ch)
  {
    switch (ch)
    {
    case OPTIONAL_OP:
    case KLEENE_OP:
    case REPEAT_OP:
      return 3;
    case CONCAT_OP:
      return 2;
    case UNION_OP:
      return 1;
    default:
      return 0;
    }
  }

}

/* -- Procedures -- */

string lexer::regex_to_postfix(const string& regex)
{
  struct shuntyard
  {

    /**
     * Runs the shuntyard algorithm.
     * https://en.wikipedia.org/wiki/Shunting-yard_algorithm
     */
    shuntyard(const string& input)
    {
      auto it = input.cbegin();
      while (it != input.cend())
      {
        char ch = *it;
        if (is_infix_operator(ch))
          push_infix_operator(ch);
        else
        {
          push_normal(ch);

          // push the implicit concatentation operator, if applicable
          auto next = it + 1;
          if (next != input.cend() && !is_operator(*next))
            push_infix_operator('.');
        }
        it++;
      }

      // pop any remaining operators
      while (!operators.empty())
        pop_operator_to_output();
    }

    /** Pushes a normal character. */
    void push_normal(char ch)
    {
      output.push_back(ch);
    }

    /** Pushes an infix operator. */
    void push_infix_operator(char op)
    {
      while (!operators.empty() &&
             ((is_left_assoc_operator(op) && operator_precedence(op) <= operator_precedence(operators.back())) ||
              (!is_left_assoc_operator(op) && operator_precedence(op) < operator_precedence(operators.back()))))
        pop_operator_to_output();
      operators.push_back(op);
    }

    /** Pops an operator to the output queue. */
    void pop_operator_to_output()
    {
      char op = operators.back();
      output.push_back(op);
      operators.pop_back();
    }

    /** Operator stack. */
    string operators;

    /** Output string. */
    string output;

  };

  shuntyard s(regex);
  return s.output;
}

string lexer::postfix_to_regex(const string& postfix)
{
  vector<string> stack;
  for (auto ch : postfix)
  {
    if (is_infix_operator(ch))
    {
      if (stack.size() < 2)
        throw runtime_error("Regular expression is invalid!");

      string lh_operand = *(stack.crbegin() + 1);
      string rh_operand = *(stack.crbegin());
      stack.pop_back();
      stack.pop_back();

      if (ch == CONCAT_OP)
        stack.push_back(lh_operand + rh_operand);
      else if (ch == UNION_OP)
        stack.push_back("(" + lh_operand + ch + rh_operand + ")");
      else
        stack.push_back(lh_operand + ch + rh_operand);
    }
    else if (is_postfix_operator(ch))
    {
      if (stack.size() < 1)
        throw runtime_error("Regular expression is invalid!");

      string operand = *(stack.crbegin());
      stack.pop_back();

      stack.push_back(operand + ch);
    }
    else
    {
      stack.push_back(string(1, ch));
    }
  }

  if (stack.size() != 1)
    throw runtime_error("Regular expression is invalid!");
  return stack.back();
}

/* -- Reference Implementation -- */

static const char* re2post(const char *re)
{
  int nalt, natom;
  static char buf[8000];
  char *dst;
  struct {
    int nalt;
    int natom;
  } paren[100], *p;

  p = paren;
  dst = buf;
  nalt = 0;
  natom = 0;
  if(strlen(re) >= sizeof buf/2)
    return NULL;
  for(; *re; re++){
    switch(*re){
    case '(':
      if(natom > 1){
        --natom;
        *dst++ = '.';
      }
      if(p >= paren+100)
        return NULL;
      p->nalt = nalt;
      p->natom = natom;
      p++;
      nalt = 0;
      natom = 0;
      break;
    case '|':
      if(natom == 0)
        return NULL;
      while(--natom > 0)
        *dst++ = '.';
      nalt++;
      break;
    case ')':
      if(p == paren)
        return NULL;
      if(natom == 0)
        return NULL;
      while(--natom > 0)
        *dst++ = '.';
      for(; nalt > 0; nalt--)
        *dst++ = '|';
      --p;
      nalt = p->nalt;
      natom = p->natom;
      natom++;
      break;
    case '*':
    case '+':
    case '?':
      if(natom == 0)
        return NULL;
      *dst++ = *re;
      break;
    default:
      if(natom > 1){
        --natom;
        *dst++ = '.';
      }
      *dst++ = *re;
      natom++;
      break;
    }
  }
  if(p != paren)
    return NULL;
  while(--natom > 0)
    *dst++ = '.';
  for(; nalt > 0; nalt--)
    *dst++ = '|';
  *dst = 0;
  return buf;
}

string lexer::regex_to_postfix_reference(const string& regex)
{
  return string(re2post(regex.c_str()));
}
