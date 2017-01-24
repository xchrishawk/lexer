/**
 * @file	main.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

/* -- Includes -- */

#include <iostream>

#include "lexical_analyzer.hpp"
#include "token.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Procedures -- */

int main(int argc, char** argv)
{
  string input = "((1 + 2) * (3 + 4))";
  lexical_analyzer lex(input);

  token tok;
  do
  {
    tok = lex.next_token();
    switch (tok.type())
    {
    case token_type::eof:
      cout << "End of file" << endl;
      break;

    default:
      cout << "Invalid token type!" << endl;
      break;
    }
  }
  while (tok.type() != token_type::eof);

  return 0;
}
