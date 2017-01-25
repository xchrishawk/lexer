/**
 * @file	expression.cpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

/* -- Includes -- */

#include <iostream>
#include <memory>

#include "expression.hpp"

/* -- Namespaces -- */

using namespace std;
using namespace lexer;

/* -- Private Procedures -- */

namespace
{

  /** Recursively prints an expression tree at the specified indentation level. */
  void internal_print_expression_tree(const unique_ptr<const expression>& expr, int indentation)
  {
    for (int idx = 0; idx < indentation; idx++)
      cout << "  ";

    switch (expr->type())
    {
    case expression_type::simple:
    {
      const auto* simple_expr = dynamic_cast<const simple_expression*>(expr.get());
      cout << simple_expr->value() << endl;
      break;
    }

    case expression_type::compound:
    {
      const auto* compound_expr = dynamic_cast<const compound_expression*>(expr.get());
      cout << operator_type_string(compound_expr->operator_type()) << endl;
      internal_print_expression_tree(compound_expr->left_expression(), indentation + 1);
      internal_print_expression_tree(compound_expr->right_expression(), indentation + 1);
      break;
    }
    }
  }

}

/* -- Procedures -- */

std::string lexer::operator_type_string(operator_type operator_type)
{
  switch (operator_type)
  {
  case lexer::operator_type::addition:
    return "+";
  case lexer::operator_type::subtraction:
    return "-";
  case lexer::operator_type::multiplication:
    return "*";
  case lexer::operator_type::division:
    return "/";
  default:
    return "unknown";
  }
}

void lexer::print_expression_tree(const unique_ptr<const expression>& expr)
{
  internal_print_expression_tree(expr, 0);
}
