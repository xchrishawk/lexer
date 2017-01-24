/**
 * @file	lexical_analyzer.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

#pragma once

/* -- Includes -- */

#include <exception>
#include <memory>
#include <string>

#include "token.hpp"

/* -- Types -- */

namespace lexer
{

  /**
   * Exception class thrown when an invalid token is encountered.
   */
  class invalid_token_error : public std::exception
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::invalid_token_error` instance with the specified information. */
    invalid_token_error(int line_number, int column_number);

    /** Returns a message for this exception. */
    virtual const char* what() const noexcept override
    {
      return m_message.c_str();
    }

    /* -- Implementation -- */

  private:

    int m_line_number;
    int m_column_number;
    std::string m_message;

  };

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
