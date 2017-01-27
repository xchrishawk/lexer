/**
 * @file	regex_constants.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/26
 */

#pragma once

/* -- Constants -- */

namespace lexer
{
  namespace regex_constants
  {

    /** Union (or) operator. */
    const char union_op = '|';

    /** Concatenation operator. */
    const char concat_op = '.';

    /** Optional character operator. */
    const char optional_op = '?';

    /** Kleene star operator. */
    const char kleene_op = '*';

    /** Repeat operator. */
    const char repeat_op = '+';

    /** Open bracket. */
    const char open_bracket = '(';

    /** Close bracket. */
    const char close_bracket = ')';

  }
}
