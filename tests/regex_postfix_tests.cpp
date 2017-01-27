/**
 * @file	regex_postfix_tests.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/27
 */

/* -- Includes -- */

#include <string>
#include <gtest/gtest.h>

#include "regex_postfix.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace testing;
using namespace lexer;

/* -- Tests -- */

/**
 * Unit test for generating postfix representations of regular expression.
 */
class regex_postfix_tests : public Test
{
};

/**
 * Verify that concatenation is correctly converted to postfix notation.
 */
TEST_F(regex_postfix_tests, concatenation)
{
  ASSERT_EQ(regex_to_postfix("a"), "a");
  ASSERT_EQ(regex_to_postfix("ab"), "ab.");
  ASSERT_EQ(regex_to_postfix("abc"), "ab.c.");
  ASSERT_EQ(regex_to_postfix("abcd"), "ab.c.d.");
}

/**
 * Verify that the alternation operatoris correctly converted to postfix notation.
 */
TEST_F(regex_postfix_tests, alternation)
{
  ASSERT_EQ(regex_to_postfix("a|b"), "ab|");
  ASSERT_EQ(regex_to_postfix("ab|c"), "ab.c|");
  ASSERT_EQ(regex_to_postfix("a|bc"), "abc.|");
  ASSERT_EQ(regex_to_postfix("((ab|cd)|(ef|gh))"), "ab.cd.|ef.gh.||");
  ASSERT_EQ(regex_to_postfix("ab|cd|ef|gh"), "ab.cd.ef.gh.|||");
}

/**
 * Verify that the optional operator is correctly converted to postfix notation.
 */
TEST_F(regex_postfix_tests, optional)
{
  ASSERT_EQ(regex_to_postfix("a?"), "a?");
  ASSERT_EQ(regex_to_postfix("a?b"), "a?b.");
  ASSERT_EQ(regex_to_postfix("ab?"), "ab?.");
  ASSERT_EQ(regex_to_postfix("ab?c"), "ab?.c.");
}

/**
 * Verify that the Kleene star operator is correctly converted to postfix notation.
 */
TEST_F(regex_postfix_tests, kleene)
{
  ASSERT_EQ(regex_to_postfix("a*"), "a*");
  ASSERT_EQ(regex_to_postfix("a*b"), "a*b.");
  ASSERT_EQ(regex_to_postfix("ab*"), "ab*.");
  ASSERT_EQ(regex_to_postfix("ab*c"), "ab*.c.");
}

/**
 * Verify that the repeat operator is correctly converted to postfix notation.
 */
TEST_F(regex_postfix_tests, repeat)
{
  ASSERT_EQ(regex_to_postfix("a+"), "a+");
  ASSERT_EQ(regex_to_postfix("a+b"), "a+b.");
  ASSERT_EQ(regex_to_postfix("ab+"), "ab+.");
  ASSERT_EQ(regex_to_postfix("ab+c"), "ab+.c.");
}
