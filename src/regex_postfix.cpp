/**
 * @file	regex_postfix.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/26
 */

/* -- Includes -- */

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "regex_constants.hpp"
#include "regex_postfix.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Private Procedures -- */

namespace
{

  /** Returns `true` if the specified operator is an infix operator. */
  bool is_infix_operator(char op)
  {
    switch (op)
    {
    case regex_constants::union_op:
    case regex_constants::concat_op:
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
    case regex_constants::optional_op:
    case regex_constants::kleene_op:
    case regex_constants::repeat_op:
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
    case regex_constants::union_op:
      return false;
    case regex_constants::concat_op:
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
    return (ch == regex_constants::open_bracket);
  }

  /** Returns `true` if the specified character is a close bracket. */
  bool is_close_bracket(char ch)
  {
    return (ch == regex_constants::close_bracket);
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
    case regex_constants::optional_op:
    case regex_constants::kleene_op:
    case regex_constants::repeat_op:
      return 3;
    case regex_constants::concat_op:
      return 2;
    case regex_constants::union_op:
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
      while (!m_operators.empty() && m_operators.back() != regex_constants::open_bracket)
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
        handle_infix_operator(regex_constants::concat_op);
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
      if (ch == regex_constants::concat_op)
        token << lh_operand << rh_operand;
      else if (ch == regex_constants::union_op)
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
