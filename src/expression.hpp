/**
 * @file	expression.hpp
 * @author	Chris Vig (chris@invictus.so)
 * @date	2017/01/24
 */

#pragma once

/* -- Includes -- */

#include <memory>
#include <string>

/* -- Types -- */

namespace lexer
{

  /**
   * Enumeration of the recognized operator types.
   */
  enum class operator_type
  {
    addition,
    subtraction,
    multiplication,
    division,
  };

  /**
   * Enumeration of the recognized expression types.
   */
  enum class expression_type
  {
    simple,
    compound,
  };

  /**
   * Abstract base class for types representing an expression.
   */
  class expression
  {

    /* -- Public Methods -- */

  public:

    /** The type of this expression. */
    virtual lexer::expression_type type() const = 0;

  };

  /**
   * Class representing a simple expression.
   */
  class simple_expression : public lexer::expression
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::simple_expression` instance with a default value. */
    simple_expression()
      : m_value()
    { }

    /** Constructs a new `lexer::simple_expression` instance with the specified value. */
    simple_expression(int value)
      : m_value(value)
    { }

    /* -- Public Methods -- */

  public:

    /** Returns the type of this expression. */
    virtual lexer::expression_type type() const override
    {
      return lexer::expression_type::simple;
    }

    /** Returns the numeric value of this expression. */
    int value() const
    {
      return m_value;
    }

    /** Sets the numeric value of this expression. */
    void set_value(int value)
    {
      m_value = value;
    }

    /* -- Implementation -- */

  private:

    int m_value;

  };

  /**
   * Class representing a compound expression.
   */
  class compound_expression : public lexer::expression
  {

    /* -- Lifecycle -- */

  public:

    /** Constructs a new `lexer::compound_expression` with default values. */
    compound_expression()
      : m_operator_type(),
        m_left_expression(),
        m_right_expression()
    { }

    /** Constructs a new `lexer::compound_expression` with the specified values. */
    compound_expression(lexer::operator_type operator_type,
                        std::unique_ptr<const lexer::expression> left_expression,
                        std::unique_ptr<const lexer::expression> right_expression)
      : m_operator_type(operator_type),
        m_left_expression(std::move(left_expression)),
        m_right_expression(std::move(right_expression))
    { }

    /* -- Public Methods -- */

  public:

    /** Returns the type of this expression. */
    virtual lexer::expression_type type() const override
    {
      return lexer::expression_type::compound;
    }

    /** Returns the operator type for this expression. */
    lexer::operator_type operator_type() const
    {
      return m_operator_type;
    }

    /** Sets the operator type for this expression. */
    void set_operator_type(lexer::operator_type operator_type)
    {
      m_operator_type = operator_type;
    }

    /** Returns the left-hand subexpression of this expression. */
    const std::unique_ptr<const lexer::expression>& left_expression() const
    {
      return m_left_expression;
    }

    /** Sets the left-hand subexpression of this expression. */
    void set_left_expression(std::unique_ptr<const lexer::expression> expression)
    {
      m_left_expression = std::move(expression);
    }

    /** Returns the right-hand subexpression of this expression. */
    const std::unique_ptr<const lexer::expression>& right_expression() const
    {
      return m_right_expression;
    }

    /** Sets the right-hand subexpression of this expression. */
    void set_right_expression(std::unique_ptr<const lexer::expression> expression)
    {
      m_right_expression = std::move(expression);
    }

    /* -- Implementation -- */

  private:

    lexer::operator_type m_operator_type;
    std::unique_ptr<const lexer::expression> m_left_expression;
    std::unique_ptr<const lexer::expression> m_right_expression;

  };

}

/* -- Procedure Prototypes -- */

namespace lexer
{

  /**
   * Returns a string representation of the specified operator type.
   */
  std::string operator_type_string(lexer::operator_type operator_type);

  /**
   * Prints an expression tree for the specified expression.
   */
  void print_expression_tree(const std::unique_ptr<const lexer::expression>& expr);

}
