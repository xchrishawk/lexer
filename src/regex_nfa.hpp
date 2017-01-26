/**
 * @file	regex_nfa.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

/* -- Types -- */

namespace lexer
{

  /**
   * Class representing a fragment in a `lexer::regex_nfa` instance.
   */
  class regex_nfa_fragment
  {

    /* -- Typedefs -- */

  public:

    /** The type used to represent a character. */
    using symbol_type = int;

    /* -- Constants -- */

  public:

    /** Constant representing an invalid link. */
    static const symbol_type invalid_symbol;

    /** Constant representing an epsilon link. */
    static const symbol_type epsilon_symbol;

    /* -- Embedded Types -- */

  private:

    /** Struct representing a link. */
    struct link
    {

      /* -- Constructor -- */

      /** Constructs a new `lexer::regex_nfa_fragment::link` instance. */
      link(symbol_type symbol)
        : symbol(symbol),
          output(nullptr)
      { }

      /* -- Fields -- */

      /** The symbol type for this link. */
      const symbol_type symbol;

      /** The NFA that this link is connected to. */
      regex_nfa_fragment* output;

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

    /** Constructs a `lexer::regex_nfa_fragment` with no valid output links (a terminal node). */
    regex_nfa_fragment()
      : link1(lexer::regex_nfa_fragment::invalid_symbol),
        link2(lexer::regex_nfa_fragment::invalid_symbol)
    { }

    /** Constructs a `lexer::regex_nfa_fragment` with a single valid output link. */
    regex_nfa_fragment(symbol_type symbol)
      : link1(symbol),
        link2(lexer::regex_nfa_fragment::invalid_symbol)
    { }

    /** Constructs a `lexer::regex_nfa_fragment` with two valid output links. */
    regex_nfa_fragment(symbol_type symbol1, symbol_type symbol2)
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

  /**
   * Class representing a non-deterministic finite automaton for a regular expression.
   */
  class regex_nfa
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::regex_nfa` instance. */
    regex_nfa(std::vector<std::unique_ptr<regex_nfa_fragment>> fragments, const regex_nfa_fragment* head)
      : m_fragments(std::move(fragments)),
        m_head(head)
    { }

    /* -- Public Methods -- */

  public:

    /** Returns a pointer to the top-level fragment for this NFA. */
    const regex_nfa_fragment* head() const
    {
      return m_head;
    }

    /* -- Implementation -- */

  private:

    std::vector<std::unique_ptr<regex_nfa_fragment>> m_fragments;
    const regex_nfa_fragment* m_head;

  };

}

/* -- Procedure Prototypes -- */

namespace lexer
{

  /**
   * Convert a regular expression to an NFA.
   */
  lexer::regex_nfa regex_to_nfa(const std::string& regex);

}
