/**
 * @file	lexical_analyzer.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

/* -- Includes -- */

#include <cctype>
#include <memory>
#include <regex>
#include <sstream>
#include <string>

#include "lexical_analyzer.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Constants -- */

namespace
{
  const regex eof_regex { "^$" };
  const regex number_regex { "^[0-9]+" };
  const regex op_regex { "^[\\+|-|\\*|/]" };
  const regex open_bracket_regex { "^\\(" };
  const regex close_bracket_regex { "^\\)" };
}

/* -- Types -- */

struct lexical_analyzer::implementation
{

  /* -- Fields -- */

  string input;
  string::const_iterator it;
  int line_number { 0 };
  int column_number { 0 };

  /* -- Methods -- */

  /** Returns `true` if the iterator is at the end of the input string. */
  bool at_eof() const
  {
    return (it == input.cend());
  }

  /** Advances the read iterator. */
  void advance(string::const_iterator::difference_type diff = 1)
  {
    for (string::const_iterator::difference_type idx = 0; idx < diff; idx++)
    {
      if (*it == '\n')
      {
        ++line_number;
        column_number = 0;
      }
      else
        ++column_number;
      ++it;
    }
  }

  /** Skips any whitespace. */
  void skip_whitespace()
  {
    while (it != input.cend() && isspace(*it))
      advance();
  }

  /** Attempts to extract a lexeme using the specified regex. */
  bool read_token(token_type type, const regex& rex, token& token)
  {
    smatch match;
    if (!regex_search(it, input.cend(), match, rex))
      return false;

    const std::string& lexeme = match[0];
    token.set_type(type);
    token.set_lexeme(lexeme);
    token.set_line_number(line_number);
    token.set_column_number(column_number);

    advance(lexeme.size());
    return true;
  }

};

/* -- Procedures -- */

invalid_token_error::invalid_token_error(int line_number, int column_number)
{
  std::ostringstream message;
  message << "Invalid token found at line " << line_number
          << ", column " << column_number << ".";
  m_message = message.str();
}

lexical_analyzer::lexical_analyzer(string input)
  : impl(make_unique<implementation>())
{
  impl->input = move(input);
  impl->it = impl->input.cbegin();
}

lexical_analyzer::~lexical_analyzer() = default;

token lexical_analyzer::next_token()
{
  impl->skip_whitespace();

  token tok;
  if (impl->read_token(token_type::eof, eof_regex, tok) ||
      impl->read_token(token_type::number, number_regex, tok) ||
      impl->read_token(token_type::op, op_regex, tok) ||
      impl->read_token(token_type::open_bracket, open_bracket_regex, tok) ||
      impl->read_token(token_type::close_bracket, close_bracket_regex, tok))
    return tok;

  throw invalid_token_error(impl->line_number, impl->column_number);
}
