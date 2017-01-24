/**
 * @file	syntax_analyzer.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <cassert>
#include <memory>
#include <stdexcept>
#include <sstream>

#include "expression.hpp"
#include "lexical_analyzer.hpp"
#include "syntax_analyzer.hpp"
#include "token.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Types -- */

struct syntax_analyzer::implementation
{

  /* -- Constructor -- */

  implementation(lexical_analyzer& lex)
    : lex(lex)
  { }

  /* -- Fields -- */

  lexical_analyzer& lex;

  /* -- Procedures -- */

  /** Gets an operator type from the specified token. */
  static operator_type get_operator_type(const token& tok)
  {
    if (tok.type() != token_type::op)
      throw parse_error(tok);

    if (tok.lexeme() == "+")
      return operator_type::addition;
    else if (tok.lexeme() == "-")
      return operator_type::subtraction;
    else if (tok.lexeme() == "*")
      return operator_type::multiplication;
    else if (tok.lexeme() == "/")
      return operator_type::division;
    else
      throw parse_error(tok);
  }

  /** Gets an integer value from the specified token. */
  static int get_value(const token& tok)
  {
    if (tok.type() != token_type::number)
      throw parse_error(tok);

    int value = 0;
    istringstream stream(tok.lexeme());
    if (!(stream >> value))
      throw parse_error(tok);

    return value;
  }

};

/* -- Procedures -- */

parse_error::parse_error(const string& lexeme, int line_number, int column_number)
{
  ostringstream message;
  message << "Unexpected token \"" << lexeme
          << "\" found at line " << line_number
          << ", column " << column_number << ".";
  m_message = message.str();
}

syntax_analyzer::syntax_analyzer(lexical_analyzer& lex)
  : impl(make_unique<implementation>(lex))
{
}

syntax_analyzer::~syntax_analyzer() = default;

unique_ptr<const expression> syntax_analyzer::next_expression()
{
  token tok = impl->lex.next_token();
  if (tok.type() == token_type::eof)
  {
    // nothing left in input stream
    return nullptr;
  }
  else if (tok.type() == token_type::number)
  {
    // simple expression
    return make_unique<simple_expression>(implementation::get_value(tok));
  }
  else if (tok.type() == token_type::open_bracket)
  {
    // get contents
    auto left = next_expression();
    auto op = implementation::get_operator_type(impl->lex.next_token());
    auto right = next_expression();

    // verify closing bracket
    if (impl->lex.next_token().type() != token_type::close_bracket)
      throw parse_error(tok);

    return make_unique<compound_expression>(op, move(left), move(right));
  }
  else
  {
    // unknown???
    throw parse_error(tok.lexeme(), tok.line_number(), tok.column_number());
  }
}
