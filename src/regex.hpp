/**
 * @file	regex.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <limits>
#include <string>

/* -- Types -- */

namespace lexer
{

  /**
   * Class representing an NFA for a regular expression.
   */
  class regex_nfa
  {

    /* -- Typedefs -- */

  public:

    /** The type used to represent a character. */
    using symbol_type = int;

    /* -- Constants -- */

  public:

    /** Constant representing an invalid link. */
    static constexpr symbol_type invalid_symbol = std::numeric_limits<symbol_type>::max();

    /** Constant representing an epsilon link. */
    static constexpr symbol_type epsilon_symbol = invalid_symbol - 1;

    /* -- Embedded Types -- */

  private:

    /** Struct representing a link. */
    struct link
    {

      /* -- Constructor -- */

      /** Constructs a new `lexer::regex_nfa::link` instance. */
      link(symbol_type symbol)
        : symbol(symbol),
          output(nullptr)
      { }

      /* -- Fields -- */

      /** The symbol type for this link. */
      const symbol_type symbol;

      /** The NFA that this link is connected to. */
      regex_nfa* output;

      /* -- Public Methods -- */

      /** Returns `true` if this is a valid link. */
      bool is_valid() const
      {
        return (symbol != invalid_symbol);
      }

      /** Returns `true` if this is an epsilon link. */
      bool is_epsilon() const
      {
        return (symbol == epsilon_symbol);
      }

    };

    /* -- Lifecycle -- */

  public:

    /** Constructs a `lexer::regex_nfa` with no valid output links (a terminal node). */
    regex_nfa()
      : link1(lexer::regex_nfa::invalid_symbol),
        link2(lexer::regex_nfa::invalid_symbol)
    { }

    /** Constructs a `lexer::regex_nfa` with a single valid output link. */
    regex_nfa(symbol_type symbol)
      : link1(symbol),
        link2(lexer::regex_nfa::invalid_symbol)
    { }

    /** Constructs a `lexer::regex_nfa` with two valid output links. */
    regex_nfa(symbol_type symbol1, symbol_type symbol2)
      : link1(symbol1),
        link2(symbol2)
    { }

    /* -- Fields -- */

  public:

    /** The first link for this NFA. */
    link link1;

    /** The second link for this NFA. */
    link link2;

    /* -- Public Methods -- */

  public:

    /** Returns `true` if this is a terminal node. */
    bool is_terminal() const
    {
      return (!link1.is_valid() && !link2.is_valid());
    }

  };

}

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

  /**
   * Convert a regular expression to an NFA.
   */
  lexer::regex_nfa* regex_to_nfa(const std::string& regex);

}
