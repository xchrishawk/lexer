/**
 * @file	regex_postfix.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/26
 */

#pragma once

/* -- Includes -- */

#include <string>

/* -- Procedure Prototypes -- */

namespace lexer
{

  /**
   * Convert a regular expression to postfix notation.
   */
  std::string regex_to_postfix(const std::string& regex);

  /**
   * Converts postfix notation to a regular expression.
   */
  std::string postfix_to_regex(const std::string& postfix);

}
