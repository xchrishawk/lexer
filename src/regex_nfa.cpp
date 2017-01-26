/**
 * @file	regex_nfa.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <cstring>
#include <iostream>
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

/* -- Private Procedures -- */

namespace
{

  /** Sets the output of a `lexer::regex_nfa` tree. */
  void set_regex_nfa_output(regex_nfa* nfa, regex_nfa* output, const regex_nfa* source)
  {
    if (nfa->link1.is_valid())
    {
      if (nfa->link1.output && nfa->link1.output != source)
        set_regex_nfa_output(nfa->link1.output, output, source);
      else if (!nfa->link1.output)
        nfa->link1.output = output;
    }
    if (nfa->link2.is_valid())
    {
      if (nfa->link2.output && nfa->link2.output != source)
        set_regex_nfa_output(nfa->link2.output, output, source);
      else if (!nfa->link2.output)
        nfa->link2.output = output;
    }
  }

  /** Sets the output of a `lexer::regex_nfa` tree. */
  void set_regex_nfa_output(regex_nfa* nfa, regex_nfa* output)
  {
    set_regex_nfa_output(nfa, output, nfa);
  }

}

/* -- Procedures -- */

regex_nfa* lexer::regex_to_nfa(const string& regex)
{
  // convert regex to postfix notation
  string postfix = regex_to_postfix(regex);

  // build NFA using stack
  vector<regex_nfa*> stack;
  for (const auto& ch : postfix)
  {
    switch (ch)
    {

    case regex_constants::concat_op:
    {
      // concatenate nodes
      auto* nfa2 = stack.back();
      stack.pop_back();
      auto* nfa1 = stack.back();
      stack.pop_back();
      set_regex_nfa_output(nfa1, nfa2);
      stack.push_back(nfa1);
      break;
    }

    case regex_constants::union_op:
    {
      // branch between nodes
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link2.output = stack.back();
      stack.pop_back();
      nfa->link1.output = stack.back();
      stack.pop_back();
      stack.push_back(nfa);
      break;
    }

    case regex_constants::optional_op:
    {
      // branch between node and epsilon
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link1.output = stack.back();
      stack.pop_back();
      stack.push_back(nfa);
      break;
    }

    case regex_constants::kleene_op:
    {
      // loop
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      nfa->link1.output = stack.back();
      stack.pop_back();
      set_regex_nfa_output(nfa->link1.output, nfa);
      stack.push_back(nfa);
      break;
    }

    case regex_constants::repeat_op:
    {
      // loop, but require passing through once
      auto* nfa = new regex_nfa { regex_nfa::epsilon_symbol, regex_nfa::epsilon_symbol };
      set_regex_nfa_output(stack.back(), nfa);
      nfa->link1.output = stack.back();
      break;
    }

    default:
    {
      // add basic node
      regex_nfa* nfa = new regex_nfa { ch };
      stack.push_back(nfa);
      break;
    }

    }
  }

  // validation
  if (stack.size() != 1)
    throw runtime_error("Regular expression is invalid!");

  // add terminal node
  auto* nfa = stack.back();
  regex_nfa* terminal = new regex_nfa { };
  set_regex_nfa_output(nfa, terminal);

  // return the final output
  return nfa;
}
