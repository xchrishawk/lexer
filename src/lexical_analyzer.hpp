/**
 * @file	lexical_analyzer.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

#pragma once

/* -- Includes -- */

#include <memory>
#include <string>

#include "token.hpp"

/* -- Types -- */

namespace lexer
{

  /**
   * Class responsible for lexical analysis.
   */
  class lexical_analyzer
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::lexical_analyzer` instance for the specified input string. */
    lexical_analyzer(std::string input);

    /** Destructor. */
    ~lexical_analyzer();

    /* -- Public Methods -- */

  public:

    /** Returns the next token from the input. */
    lexer::token next_token();

    /* -- Implementation -- */

  private:

    struct implementation;
    std::unique_ptr<implementation> impl;

  };

}
