/**
 * @file	lexical_analyzer.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

/* -- Includes -- */

#include <memory>
#include <sstream>
#include <string>

#include "lexical_analyzer.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Types -- */

struct lexical_analyzer::implementation
{

  /* -- Fields -- */

  string input;
  string::const_iterator it;

};

/* -- Procedures -- */

lexical_analyzer::lexical_analyzer(string input)
  : impl(make_unique<implementation>())
{
  impl->input = move(input);
  impl->it = impl->input.cbegin();
}

lexical_analyzer::~lexical_analyzer() = default;

token lexical_analyzer::next_token()
{
  token tok(token_type::eof);
  return tok;
}
