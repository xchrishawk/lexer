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
#include "regex.hpp"
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
  for (int idx = 1; idx < argc; idx++)
    parse_string(argv[idx]);

  const string regex = "abc|d+ef|gh?i|xyz*";
  cout << regex << endl;

  const string their_postfix = std::string(regex_to_postfix_reference(regex));
  cout << their_postfix << endl;

  const string their_unpostfix = postfix_to_regex(their_postfix);
  cout << their_unpostfix << endl;

  const string our_postfix = std::string(regex_to_postfix(regex));
  cout << our_postfix << endl;

  const string our_unpostfix = std::string(postfix_to_regex(our_postfix));
  cout << our_unpostfix << endl;

  return 0;
}
