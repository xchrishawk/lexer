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
    invalid,
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
      : m_type(lexer::token_type::invalid),
        m_lexeme()
    { }

    /** Constructs a new `lexer::token` instance. */
    token(lexer::token_type type)
      : m_type(type),
        m_lexeme()
    { }

    /** Constructs a new `lexer::token` instance. */
    token(lexer::token_type type, std::string lexeme)
      : m_type(type),
        m_lexeme(std::move(lexeme))
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

    /* -- Implementation -- */

  private:

    lexer::token_type m_type;
    std::string m_lexeme;

  };

}
