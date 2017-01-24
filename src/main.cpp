/**
 * @file	main.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

/* -- Includes -- */

#include <exception>
#include <iostream>

#include "lexical_analyzer.hpp"
#include "token.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Private Procedures -- */

namespace
{

  /** Tokenizes a string and prints the tokens. */
  void tokenize_string(const std::string& input)
  {
    try
    {
      lexical_analyzer lex(input);
      bool continue_reading = true;
      do
      {
        token tok = lex.next_token();
        switch (tok.type())
        {
        case token_type::eof:
          cout << "End of file";
          continue_reading = false;
          break;

        case token_type::open_bracket:
          cout << "Open bracket";
          break;

        case token_type::close_bracket:
          cout << "Close bracket";
          break;

        case token_type::number:
          cout << "Number: " << tok.lexeme();
          break;

        case token_type::op:
          cout << "Operator: " << tok.lexeme();
          break;
        }
        cout << " (line " << tok.line_number()
             << ", column " << tok.column_number()
             << ")" << endl;
      }
      while (continue_reading);
    }
    catch (const exception& ex)
    {
      cerr << ex.what() << endl;
    }
  }
}

/* -- Procedures -- */

int main(int argc, char** argv)
{
  for (int idx = 1; idx < argc; idx++)
    tokenize_string(argv[idx]);
  return 0;
}
