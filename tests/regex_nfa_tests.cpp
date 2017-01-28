/**
 * @file	regex_nfa_tests.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/26
 */

/* -- Includes -- */

#include <string>
#include <gtest/gtest.h>

#include "regex_nfa.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace testing;
using namespace lexer;

/* -- Tests -- */

/**
 * Unit test for generating NFAs from regular expressions.
 */
class regex_nfa_tests : public Test
{
protected:

  /** Assert that a link is a valid epsilon link. */
  template <typename TLink>
  void assert_valid_epsilon_link(const TLink& link)
  {
    ASSERT_EQ(link.is_valid(), true);
    ASSERT_EQ(link.is_epsilon(), true);
    ASSERT_NE(link.output, nullptr);
  }

  /** Assert that a link is a valid epsilon link pointing to the specified destination. */
  template <typename TLink>
  void assert_valid_epsilon_link(const TLink& link, const regex_nfa_fragment* dest)
  {
    assert_valid_epsilon_link(link);
    ASSERT_EQ(link.output, dest);
  }

  /** Assert that a link is a valid symbol link. */
  template <typename TLink>
  void assert_valid_symbol_link(const TLink& link, regex_nfa_fragment::symbol_type symbol)
  {
    ASSERT_EQ(link.is_valid(), true);
    ASSERT_EQ(link.is_epsilon(), false);
    ASSERT_EQ(link.symbol, symbol);
    ASSERT_NE(link.output, nullptr);
  }

  /** Assert that a link is a valid symbol link pointing to the specified destination. */
  template <typename TLink>
  void assert_valid_symbol_link(const TLink& link, regex_nfa_fragment::symbol_type symbol, const regex_nfa_fragment* dest)
  {
    assert_valid_symbol_link(link, symbol);
    ASSERT_EQ(link.output, dest);
  }

  /** Assert that a link is an invalid link. */
  template <typename TLink>
  void assert_invalid_link(const TLink& link)
  {
    ASSERT_EQ(link.is_valid(), false);
    ASSERT_EQ(link.is_epsilon(), false);
    ASSERT_EQ(link.output, nullptr);
  }

  /** Assert that a fragment is a terminal fragment. */
  void assert_terminal(const regex_nfa_fragment* frag)
  {
    assert_invalid_link(frag->link1);
    assert_invalid_link(frag->link2);
    ASSERT_EQ(frag->is_terminal(), true);
  }

};

/**
 * Verifies that the generated NFA is correct for the "abc" regular expression.
 *
 *      a      b      c
 *   0 ---> 1 ---> 2 ---> |3|
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
 *
 *      a     eps     b      d
 *   0 ---> 1 ---> 2 ---> 4 ---> |5|
 *          |             ^
 *          | eps     c   |
 *          +----> 3 -----+
 */
TEST_F(regex_nfa_tests, alternation)
{
  auto nfa = regex_to_nfa("a(b|c)d");

  auto frag0 = nfa.head();
  assert_valid_symbol_link(frag0->link1, 'a');
  assert_invalid_link(frag0->link2);

  auto frag1 = frag0->link1.output;
  assert_valid_epsilon_link(frag1->link1);
  assert_valid_epsilon_link(frag1->link2);

  auto frag2 = frag1->link1.output;
  assert_valid_symbol_link(frag2->link1, 'b');
  assert_invalid_link(frag2->link2);

  auto frag4 = frag2->link1.output;
  assert_valid_symbol_link(frag4->link1, 'd');
  assert_invalid_link(frag4->link2);

  auto frag3 = frag1->link2.output;
  assert_valid_symbol_link(frag3->link1, 'c', frag4);
  assert_invalid_link(frag3->link2);

  auto frag5 = frag4->link1.output;
  assert_terminal(frag5);
}

/**
 * Verifies that the generated NFA is correct for the "ab?c" regular expression.
 *
 *      a     eps     b      c
 *   0 ---> 1 ---> 2 ---> 3 ---> |4|
 *          |             ^
 *          |     eps     |
 *          +-------------+
 */
TEST_F(regex_nfa_tests, optional)
{
  auto nfa = regex_to_nfa("ab?c");

  auto frag0 = nfa.head();
  assert_valid_symbol_link(frag0->link1, 'a');
  assert_invalid_link(frag0->link2);

  auto frag1 = frag0->link1.output;
  assert_valid_epsilon_link(frag1->link1);
  assert_valid_epsilon_link(frag1->link2);

  auto frag2 = frag1->link1.output;
  assert_valid_symbol_link(frag2->link1, 'b');
  assert_invalid_link(frag2->link2);

  auto frag3 = frag2->link1.output;
  assert_valid_symbol_link(frag3->link1, 'c');
  assert_invalid_link(frag3->link2);
  ASSERT_EQ(frag1->link2.output, frag3);

  auto frag4 = frag3->link1.output;
  assert_terminal(frag4);
}

/**
 * Verifies that the generated NFA is correct for the "ab*c" regular expression.
 *
 *               b
 *          +--------+
 *          |        |
 *      a   v   eps  |
 *   0 ---> 1 -----> 2
 *          |
 *          | eps     c
 *          +----> 3 ---> |4|
 */
TEST_F(regex_nfa_tests, kleene)
{
  auto nfa = regex_to_nfa("ab*c");

  auto frag0 = nfa.head();
  assert_valid_symbol_link(frag0->link1, 'a');
  assert_invalid_link(frag0->link2);

  auto frag1 = frag0->link1.output;
  assert_valid_epsilon_link(frag1->link1);
  assert_valid_epsilon_link(frag1->link2);

  auto frag2 = frag1->link1.output;
  assert_valid_symbol_link(frag2->link1, 'b', frag1);
  assert_invalid_link(frag2->link2);

  auto frag3 = frag1->link2.output;
  assert_valid_symbol_link(frag3->link1, 'c');
  assert_invalid_link(frag3->link2);

  auto frag4 = frag3->link1.output;
  assert_terminal(frag4);
}

/**
 * Verifies that the generated NFA is correct for the "ab+c" regular expression.
 *
 *            eps
 *          +------+
 *          |      |
 *      a   v  b   |  eps     c
 *   0 ---> 1 ---> 2 ---> 3 ---> |4|
 */
TEST_F(regex_nfa_tests, repeat)
{
  auto nfa = regex_to_nfa("ab+c");

  auto frag0 = nfa.head();
  assert_valid_symbol_link(frag0->link1, 'a');
  assert_invalid_link(frag0->link2);

  auto frag1 = frag0->link1.output;
  assert_valid_symbol_link(frag1->link1, 'b');
  assert_invalid_link(frag1->link2);

  auto frag2 = frag1->link1.output;
  assert_valid_epsilon_link(frag2->link1, frag1);
  assert_valid_epsilon_link(frag2->link2);

  auto frag3 = frag2->link2.output;
  assert_valid_symbol_link(frag3->link1, 'c');
  assert_invalid_link(frag3->link2);

  auto frag4 = frag3->link1.output;
  assert_terminal(frag4);
}

/**
 * Unit test for the `regex_match` method.
 */
class regex_match_tests : public Test
{
};

/**
 * Verify that the `lexer::regex_match` function correctly matches the concatenation operator.
 */
TEST_F(regex_match_tests, concatenation)
{
  static const string REGEX = "abc";

  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the union operator.
 */
TEST_F(regex_match_tests, alternation)
{
  static const string REGEX = "a(b|c)d";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abd"));
  EXPECT_TRUE(regex_match(REGEX, "acd"));
  EXPECT_FALSE(regex_match(REGEX, "xbd"));
  EXPECT_FALSE(regex_match(REGEX, "axd"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the optional operator at the
 * beginning of a string.
 */
TEST_F(regex_match_tests, optional_beginning)
{
  static const string REGEX = "a?bc";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "bc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the optional operator in the
 * middle of a string.
 */
TEST_F(regex_match_tests, optional_middle)
{
  static const string REGEX = "ab?c";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_TRUE(regex_match(REGEX, "ac"));
  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the optional operator at the
 * end of a string.
 */
TEST_F(regex_match_tests, optional_end)
{
  static const string REGEX = "abc?";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_TRUE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the Kleene star operator at the
 * beginning of a string.
 */
TEST_F(regex_match_tests, kleene_beginning)
{
  static const string REGEX = "a*bc";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "bc"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "aabc"));
  EXPECT_TRUE(regex_match(REGEX, "aaabc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the Kleene star operator in the
 * middle of a string.
 */
TEST_F(regex_match_tests, kleene_middle)
{
  static const string REGEX = "ab*c";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_TRUE(regex_match(REGEX, "ac"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "abbc"));
  EXPECT_TRUE(regex_match(REGEX, "abbbc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the Kleene star operator at the
 * end of a string.
 */
TEST_F(regex_match_tests, kleene_end)
{
  static const string REGEX = "abc*";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_TRUE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "abcc"));
  EXPECT_TRUE(regex_match(REGEX, "abccc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the repeat operator at the
 * beginning of a string.
 */
TEST_F(regex_match_tests, repeat_beginning)
{
  static const string REGEX = "a+bc";

  EXPECT_FALSE(regex_match(REGEX, "b"));
  EXPECT_FALSE(regex_match(REGEX, "bc"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "aabc"));
  EXPECT_TRUE(regex_match(REGEX, "aaabc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the repeat operator in the
 * middle of a string.
 */
TEST_F(regex_match_tests, repeat_middle)
{
  static const string REGEX = "ab+c";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_FALSE(regex_match(REGEX, "ac"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "abbc"));
  EXPECT_TRUE(regex_match(REGEX, "abbbc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches the repeat operator at the
 * end of a string.
 */
TEST_F(regex_match_tests, repeat_end)
{
  static const string REGEX = "abc+";

  EXPECT_FALSE(regex_match(REGEX, "a"));
  EXPECT_FALSE(regex_match(REGEX, "ab"));
  EXPECT_TRUE(regex_match(REGEX, "abc"));
  EXPECT_TRUE(regex_match(REGEX, "abcc"));
  EXPECT_TRUE(regex_match(REGEX, "abccc"));
  EXPECT_FALSE(regex_match(REGEX, "xbc"));
  EXPECT_FALSE(regex_match(REGEX, "axc"));
  EXPECT_FALSE(regex_match(REGEX, "abx"));
}

/**
 * Verify that the `lexer::regex_match` function correctly matches strings using alternation.
 */
TEST_F(regex_match_tests, word_alternation)
{
  static const string REGEX = "constexpr|static_cast|namespace";

  EXPECT_TRUE(regex_match(REGEX, "constexpr"));
  EXPECT_FALSE(regex_match(REGEX, "cosntexpr"));
  EXPECT_TRUE(regex_match(REGEX, "static_cast"));
  EXPECT_FALSE(regex_match(REGEX, "sttaic_cast"));
  EXPECT_TRUE(regex_match(REGEX, "namespace"));
  EXPECT_FALSE(regex_match(REGEX, "namespcae"));
}

TEST_F(regex_match_tests, complex)
{
  static const string REGEX = "(abc|d+e)(xyz?|123)";

  EXPECT_TRUE(regex_match(REGEX, "abcxyz"));
//  EXPECT_TRUE(regex_match(REGEX, "abcxy"));



  EXPECT_TRUE(regex_match(REGEX, "dexyz"));
  EXPECT_TRUE(regex_match(REGEX, "ddexyz"));
}
