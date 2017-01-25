/**
 * @file	regex.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <string>

/* -- Procedure Prototypes -- */

namespace lexer
{

  /**
   * Convert a regular expression to postfix notation.
   *
   * @note
   * This is a reference implementation from:
   * https://swtch.com/~rsc/regexp/nfa.c.txt
   */
  std::string regex_to_postfix_reference(const std::string& regex);

  /**
   * Convert a regular expression to postfix notation.
   */
  std::string regex_to_postfix(const std::string& regex);

  /**
   * Converts postfix notation to a regular expression.
   */
  std::string postfix_to_regex(const std::string& postfix);

}
