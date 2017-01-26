/**
 * @file	regex_nfa_tests.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/26
 */

/* -- Includes -- */

#include <gtest/gtest.h>
#include "regex_nfa.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace testing;
using namespace lexer;

/* -- Private Procedures -- */

namespace
{

  template <typename TLink>
  void assert_valid_epsilon_link(const TLink& link)
  {
    ASSERT_EQ(link.is_valid(), true);
    ASSERT_EQ(link.is_epsilon(), true);
    ASSERT_NE(link.output, nullptr);
  }

  template <typename TLink>
  void assert_valid_epsilon_link(const TLink& link, const regex_nfa_fragment* dest)
  {
    assert_valid_epsilon_link(link);
    ASSERT_EQ(link.output, dest);
  }

  template <typename TLink>
  void assert_valid_symbol_link(const TLink& link, regex_nfa_fragment::symbol_type symbol)
  {
    ASSERT_EQ(link.is_valid(), true);
    ASSERT_EQ(link.is_epsilon(), false);
    ASSERT_EQ(link.symbol, symbol);
    ASSERT_NE(link.output, nullptr);
  }

  template <typename TLink>
  void assert_valid_symbol_link(const TLink& link, regex_nfa_fragment::symbol_type symbol, const regex_nfa_fragment* dest)
  {
    assert_valid_symbol_link(link, symbol);
    ASSERT_EQ(link.output, dest);
  }

  template <typename TLink>
  void assert_invalid_link(const TLink& link)
  {
    ASSERT_EQ(link.is_valid(), false);
    ASSERT_EQ(link.is_epsilon(), false);
    ASSERT_EQ(link.output, nullptr);
  }

  void assert_terminal(const regex_nfa_fragment* frag)
  {
    assert_invalid_link(frag->link1);
    assert_invalid_link(frag->link2);
    ASSERT_EQ(frag->is_terminal(), true);
  }

}

/* -- Tests -- */

/**
 * Unit test for generating NFAs from regular expressions.
 */
class regex_nfa_tests : public Test
{
};

/**
 * Verifies that the generated NFA is correct for the "abc" regular expression.
 */
TEST_F(regex_nfa_tests, concatenation)
{
  auto nfa = regex_to_nfa("abc");

  auto frag0 = nfa.head();
  assert_valid_symbol_link(frag0->link1, 'a');
  assert_invalid_link(frag0->link2);

  auto frag1 = frag0->link1.output;
  assert_valid_symbol_link(frag1->link1, 'b');
  assert_invalid_link(frag1->link2);

  auto frag2 = frag1->link1.output;
  assert_valid_symbol_link(frag2->link1, 'c');
  assert_invalid_link(frag2->link2);

  auto frag3 = frag2->link1.output;
  assert_terminal(frag3);
}

/**
 * Verifies that the generated NFA is correct for the "a(b|c)d" regular expression.
 */
TEST_F(regex_nfa_tests, alternation)
{
  auto nfa = regex_to_nfa("a(b|c)d");
  // TODO
}

/**
 * Verifies that the generated NFA is correct for the "ab?c" regular expression.
 */
TEST_F(regex_nfa_tests, optional)
{
  auto nfa = regex_to_nfa("ab?c");
  // TODO
}

/**
 * Verifies that the generated NFA is correct for the "ab*c" regular expression.
 */
TEST_F(regex_nfa_tests, kleene)
{
  auto nfa = regex_to_nfa("ab*c");
  // TODO
}

/**
 * Verifies that the generated NFA is correct for the "ab+c" regular expression.
 */
TEST_F(regex_nfa_tests, repeat)
{
  auto nfa = regex_to_nfa("ab+c");
  // TODO
}
