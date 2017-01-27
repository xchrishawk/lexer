/**
 * @file	regex_nfa.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "regex_constants.hpp"
#include "regex_nfa.hpp"
#include "regex_postfix.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Constants -- */

const regex_nfa_fragment::symbol_type regex_nfa_fragment::invalid_symbol = std::numeric_limits<symbol_type>::max();
const regex_nfa_fragment::symbol_type regex_nfa_fragment::epsilon_symbol = std::numeric_limits<symbol_type>::max() - 1;

/* -- Private Procedures -- */

namespace
{

  /**
   * Recursively sets the output of a `lexer::regex_nfa_fragment` tree.
   *
   * @note
   * The `prev` parameter is to prevent infinitely recursing in case of loops. Since all loops in
   * the regular expression NFA construction only point to the "previous" node, we only need to
   * check one level backwards to ensure that we don't loop around.
   */
  void set_output(regex_nfa_fragment* nfa, regex_nfa_fragment* output, const regex_nfa_fragment* prev)
  {
    if (nfa->link1.is_valid())
    {
      if (nfa->link1.output && nfa->link1.output != prev)
        set_output(nfa->link1.output, output, nfa);
      else if (!nfa->link1.output)
        nfa->link1.output = output;
    }
    if (nfa->link2.is_valid())
    {
      if (nfa->link2.output && nfa->link2.output != prev)
        set_output(nfa->link2.output, output, nfa);
      else if (!nfa->link2.output)
        nfa->link2.output = output;
    }
  }

  /** Sets the output of a `lexer::regex_nfa_fragment` tree. */
  void set_output(regex_nfa_fragment* nfa, regex_nfa_fragment* output)
  {
    set_output(nfa, output, nullptr);
  }

}

/* -- Procedures -- */

regex_nfa lexer::regex_to_nfa(const string& regex)
{
  // convert regex to postfix notation
  string postfix = regex_to_postfix(regex);

  // container for pointers to all of the fragments we need to allocate
  vector<unique_ptr<regex_nfa_fragment>> fragments;

  // processing stack
  vector<regex_nfa_fragment*> stack;

  // local procedure to create and return a new fragment
  auto new_fragment = [&] (auto... args) -> regex_nfa_fragment* {
    fragments.push_back(make_unique<regex_nfa_fragment>(args...));
    return fragments.back().get();
  };

  // local procedure to push a fragment onto the stack
  auto push_fragment = [&] (regex_nfa_fragment* frag) {
    stack.push_back(frag);
  };

  // local procedure to pop a fragment off of the stack
  auto pop_fragment = [&] () -> regex_nfa_fragment* {
    auto* frag = stack.back();
    stack.pop_back();
    return frag;
  };

  // loop through each character in regular expression
  // this is largely based on https://swtch.com/~rsc/regexp/regexp1.html
  for (const auto& ch : postfix)
  {
    switch (ch)
    {

    case regex_constants::concat_op:
    {
      // - E1 and E2 are popped off stack
      // - all links are epsilon links
      //
      //    IN -> E1 -> E2 -> OUT
      //
      auto e2 = pop_fragment();
      auto e1 = pop_fragment();
      set_output(e1, e2);
      push_fragment(e1);
      break;
    }

    case regex_constants::union_op:
    {
      // - E1 and E2 are popped off stack
      // - all links are epsilon links
      //
      //    IN -> NFA -> E1 -> OUT
      //           |
      //           +---> E2 -> OUT
      //
      auto nfa = new_fragment(regex_nfa_fragment::epsilon_symbol, regex_nfa_fragment::epsilon_symbol);
      nfa->link2.output = pop_fragment();
      nfa->link1.output = pop_fragment();
      push_fragment(nfa);
      break;
    }

    case regex_constants::optional_op:
    {
      // - E is popped off stack
      // - all links are epsilon links
      //
      //    IN -> NFA -> E -> OUT
      //           |
      //           +--------> OUT
      //
      auto nfa = new_fragment(regex_nfa_fragment::epsilon_symbol, regex_nfa_fragment::epsilon_symbol);
      nfa->link1.output = pop_fragment();
      push_fragment(nfa);
      break;
    }

    case regex_constants::kleene_op:
    {
      // - E is popped off stack
      // - all links are epsilon links
      //
      //           +-----+
      //           |     |
      //           v     |
      //    IN -> NFA -> E
      //           |
      //           +---> OUT
      //
      auto nfa = new_fragment(regex_nfa_fragment::epsilon_symbol, regex_nfa_fragment::epsilon_symbol);
      nfa->link1.output = pop_fragment();
      set_output(nfa->link1.output, nfa);
      push_fragment(nfa);
      break;
    }

    case regex_constants::repeat_op:
    {
      // - E is popped off stack
      // - all links are epsilon links
      //
      //          +-----+
      //          |     |
      //          v     |
      //    IN -> E -> NFA -> OUT
      //
      auto nfa = new_fragment(regex_nfa_fragment::epsilon_symbol, regex_nfa_fragment::epsilon_symbol);
      auto e = pop_fragment();
      set_output(e, nfa);
      nfa->link1.output = e;
      push_fragment(e);
      break;
    }

    default:
    {
      // - link requires a matching symbol
      //
      //       ch
      //    IN -> OUT
      //
      auto nfa = new_fragment(ch);
      push_fragment(nfa);
      break;
    }

    }
  }

  // validation/sanity check
  if (stack.size() != 1)
    throw runtime_error("Regular expression is invalid!");

  // add terminal node to complete the NFA
  auto head = stack.back();
  auto terminal = new_fragment();
  set_output(head, terminal);

  // return the final object
  return regex_nfa(move(fragments), head);
}
