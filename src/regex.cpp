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

  /** Sets the output of a `lexer::regex_nfa` tree. */
  void set_regex_nfa_output(regex_nfa* nfa, regex_nfa* output, const regex_nfa* source)
  {
    if (nfa->link1.is_valid())
    {
      if (nfa->link1.output && nfa->link1.output != source)
        set_regex_nfa_output(nfa->link1.output, output, source);
      else if (!nfa->link1.output)
        nfa->link1.output = output;
    }
    if (nfa->link2.is_valid())
    {
      if (nfa->link2.output && nfa->link2.output != source)
        set_regex_nfa_output(nfa->link2.output, output, source);
      else if (!nfa->link2.output)
        nfa->link2.output = output;
    }
  }

  /** Sets the output of a `lexer::regex_nfa` tree. */
  void set_regex_nfa_output(regex_nfa* nfa, regex_nfa* output)
  {
    set_regex_nfa_output(nfa, output, nfa);
  }

}

/* -- Procedures -- */

regex_nfa* lexer::regex_to_nfa(const string& regex)
{
  // convert regex to postfix notation
  string postfix = regex_to_postfix(regex);

  // build NFA using stack
  vector<regex_nfa*> stack;
  for (const auto& ch : postfix)
  {
    switch (ch)
    {

    case CONCAT_OP:
    {
      // concatenate nodes
      auto* nfa2 = stack.back();
      stack.pop_back();
      auto* nfa1 = stack.back();
      stack.pop_back();
      set_regex_nfa_output(nfa1, nfa2);
      stack.push_back(nfa1);
      break;
    }

    case UNION_OP:
    {
      // branch between nodes
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link2.output = stack.back();
      stack.pop_back();
      nfa->link1.output = stack.back();
      stack.pop_back();
      stack.push_back(nfa);
      break;
    }

    case OPTIONAL_OP:
    {
      // branch between node and epsilon
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link1.output = stack.back();
      stack.pop_back();
      stack.push_back(nfa);
      break;
    }

    case KLEENE_OP:
    {
      // loop
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link1.output = stack.back();
      stack.pop_back();
      set_regex_nfa_output(nfa->link1.output, nfa);
      stack.push_back(nfa);
      break;
    }

    case REPEAT_OP:
    {
      // loop, but require passing through once
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      set_regex_nfa_output(stack.back(), nfa);
      nfa->link1.output = stack.back();
      break;
    }

    default:
    {
      // add basic node
      regex_nfa* nfa = new regex_nfa { ch };
      stack.push_back(nfa);
      break;
    }

    }
  }

  // validation
  if (stack.size() != 1)
    throw runtime_error("Regular expression is invalid!");

  // add terminal node
  auto* nfa = stack.back();
  regex_nfa* terminal = new regex_nfa { };
  set_regex_nfa_output(nfa, terminal);

  // return the final output
  return nfa;
}

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
