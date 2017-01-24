/**
 * @file	token.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/23
 */

#pragma once

/* -- Includes -- */

#include <string>
#include <utility>

/* -- Types -- */

namespace lexer
{

  /**
   * Enumeration of token types recognized by the program.
   */
  enum class token_type
  {
    eof,
    number,
    op,
    open_bracket,
    close_bracket,
  };

  /**
   * Class representing a token from the input stream.
   */
  class token
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::token` instance. */
    token()
      : m_type(),
        m_lexeme(),
        m_line_number(),
        m_column_number()
    { }

    /* -- Public Methods -- */

  public:

    /** Returns the type of this token. */
    lexer::token_type type() const
    {
      return m_type;
    }

    /** Sets the type of this token. */
    void set_type(lexer::token_type type)
    {
      m_type = type;
    }

    /** Returns the string representation of this token. */
    std::string lexeme() const
    {
      return m_lexeme;
    }

    /** Sets the string representation of this token. */
    void set_lexeme(std::string lexeme)
    {
      m_lexeme = std::move(lexeme);
    }

    /** Returns the line number at which this token was found. */
    int line_number() const
    {
      return m_line_number;
    }

    /** Sets the line number at which this token was found. */
    void set_line_number(int line_number)
    {
      m_line_number = line_number;
    }

    /** Returns the column number at which this token was found. */
    int column_number() const
    {
      return m_column_number;
    }

    /** Sets the column number at which this token was found. */
    void set_column_number(int column_number)
    {
      m_column_number = column_number;
    }

    /* -- Implementation -- */

  private:

    lexer::token_type m_type;
    std::string m_lexeme;
    int m_line_number;
    int m_column_number;

  };

}
