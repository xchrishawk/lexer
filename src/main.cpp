/**
 * @file	main.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

/* -- Includes -- */

#include <exception>
#include <iostream>

#include "expression.hpp"
#include "lexical_analyzer.hpp"
#include "regex_nfa.hpp"
#include "regex_postfix.hpp"
#include "syntax_analyzer.hpp"
#include "token.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Private Procedures -- */

namespace
{

  /** Parses a string and prints the syntax tree. */
  void parse_string(const std::string& input)
  {
    try
    {
      lexical_analyzer lex(input);
      syntax_analyzer syn(lex);

      bool continue_reading = true;
      do
      {
        auto expr = syn.next_expression();
        if (expr)
          print_expression_tree(expr);
        else
          continue_reading = false;
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
  static const string REGEX = "a?bc";

  cout << boolalpha;
  cout << regex_match(REGEX, "abc") << endl;

  return 0;
}
