/**
 * @file	syntax_analyzer.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

#pragma once

/* -- Includes -- */

#include <exception>
#include <memory>
#include <string>

#include "lexical_analyzer.hpp"
#include "token.hpp"

/* -- Types -- */

namespace lexer
{

  class expression;

  /**
   * Exception class thrown when an expression cannot be parsed.
   */
  class parse_error : public std::exception
  {

  public:

    /** Constructs a new `lexer::parse_error` instance using information from the specified token. */
    parse_error(const token& tok)
      : parse_error(tok.lexeme(), tok.line_number(), tok.column_number())
    { }

    /** Constructs a new `lexer::parse_error` instance with the specified information. */
    parse_error(const std::string& lexeme, int line_number, int column_number);

    /** Returns a message for this exception. */
    virtual const char* what() const noexcept override
    {
      return m_message.c_str();
    }

    /* -- Implementation -- */

  private:

    std::string m_message;

  };

  /**
   * Class responsible for syntax analysis.
   */
  class syntax_analyzer
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::syntax_analyzer` with the specified parameters. */
    syntax_analyzer(lexer::lexical_analyzer& lex);

    /** Destructor. */
    ~syntax_analyzer();

    /* -- Public Methods -- */

  public:

    /** Parses the next top-level expression. */
    std::unique_ptr<const lexer::expression> next_expression();

    /* -- Implementation -- */

  private:

    struct implementation;
    std::unique_ptr<implementation> impl;

  };

}
