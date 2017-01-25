/**
 * @file	regex.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <cstring>
#include <iostream>
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

  // brackets
  const char OPEN_BRACKET = '(';
  const char CLOSE_BRACKET = ')';
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

  /** Returns `true` if the specified character is an open bracket. */
  bool is_open_bracket(char ch)
  {
    return (ch == OPEN_BRACKET);
  }

  /** Returns `true` if the specified character is a close bracket. */
  bool is_close_bracket(char ch)
  {
    return (ch == CLOSE_BRACKET);
  }

  /** Returns `true` if the specified character is a normal character. */
  bool is_normal(char ch)
  {
    return (!is_operator(ch) &&
            !is_open_bracket(ch) &&
            !is_close_bracket(ch));
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
  class shuntyard
  {
  public:

    /**
     * Runs the shuntyard algorithm.
     * https://en.wikipedia.org/wiki/Shunting-yard_algorithm
     */
    shuntyard(const string& input)
      : m_input(input),
        m_it(m_input.cbegin())
    {
      while (m_it != m_input.cend())
      {
        char ch = *m_it;
        if (is_infix_operator(ch))
          handle_infix_operator(ch);
        else if (is_open_bracket(ch))
          handle_open_bracket(ch);
        else if (is_close_bracket(ch))
          handle_close_bracket(ch);
        else
          handle_normal(ch);
        m_it++;
      }

      // pop any remaining operators
      while (!m_operators.empty())
        pop_operator_to_output();
    }

    /** Returns the final output string. */
    string output() const
    {
      return m_output;
    }

  private:

    /** Handles a normal character. */
    void handle_normal(char ch)
    {
      m_output.push_back(ch);
      add_implicit_concat_if_needed();
    }

    /** Handles an open bracket. */
    void handle_open_bracket(char ch)
    {
      m_operators.push_back(ch);
    }

    /** Handles a close bracket. */
    void handle_close_bracket(char ch)
    {
      while (!m_operators.empty() && m_operators.back() != OPEN_BRACKET)
        pop_operator_to_output();
      if (m_operators.empty())
        throw runtime_error("Unmatched parentheses!");
      m_operators.pop_back();
      add_implicit_concat_if_needed();
    }

    /** Handles an infix operator. */
    void handle_infix_operator(char op)
    {
      while (!m_operators.empty() &&
             ((is_left_assoc_operator(op) &&
               operator_precedence(op) <= operator_precedence(m_operators.back())) ||
              (!is_left_assoc_operator(op) &&
               operator_precedence(op) < operator_precedence(m_operators.back()))))
        pop_operator_to_output();
      m_operators.push_back(op);
    }

    /** Adds the implicit concatentation operator, if needed. */
    void add_implicit_concat_if_needed()
    {
      auto next = m_it + 1;
      if (next != m_input.end() && (is_normal(*next) || is_open_bracket(*next)))
        handle_infix_operator(CONCAT_OP);
    }

    /** Pops an operator to the output queue. */
    void pop_operator_to_output()
    {
      char op = m_operators.back();
      m_output.push_back(op);
      m_operators.pop_back();
    }

    const string m_input;
    string::const_iterator m_it;
    string m_operators;
    string m_output;

  };

  shuntyard s(regex);
  return s.output();
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

      const auto& lh_operand = *(stack.crbegin() + 1);
      const auto& rh_operand = *(stack.crbegin());
      ostringstream token;
      if (ch == CONCAT_OP)
        token << lh_operand << rh_operand;
      else if (ch == UNION_OP)
        token << "(" << lh_operand << ch << rh_operand << ")";
      else
        token << lh_operand << ch << rh_operand;

      stack.pop_back();
      stack.pop_back();
      stack.push_back(token.str());
    }
    else if (is_postfix_operator(ch))
    {
      if (stack.size() < 1)
        throw runtime_error("Regular expression is invalid!");

      const auto& operand = *(stack.crbegin());
      ostringstream token;
      token << operand << ch;

      stack.pop_back();
      stack.push_back(token.str());
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
