#include "Analex.h"
#include "TabIdent.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef ANASEM_H
#define ANASEM_H

/**
 * @brief Reports a formatted semantic error and terminates compilation.
 * @param formato A printf-style format string.
 * @param ... Variable arguments for the format string.
 */
void erro_semantic(const char* formato, ...);

/**
 * @brief Converts a value from the DATA_TYPE enum to its string representation.
 * @param type The type of data to be converted.
 * @return A constant string representing the type.
 */
const char* typeToString(DATA_TYPE type);

/**
 * @brief Checks type compatibility in an assignment statement.
 * And allows some implicit conversions (e.g., int -> float).
 * @param type_lhs Left-hand type (variable).
 * @param type_rhs Right-hand type (expression).
 * @param nome_lhs Name of the variable on the left side, for error messages.
 */
void checkCompatibilityAssignment(DATA_TYPE type_lhs, DATA_TYPE type_rhs, const char* nome_lhs);

/**
 * @brief Checks type compatibility for arithmetic operators (+, -, *, /)
 * And returns the type resulting from the operation (with type promotion).
 * @param op1 Type of the first operand.
 * @param op2 Type of the second operand.
 * @return The resulting data type (e.g., FLOAT_TYPE if one of the operands is float).
 */
DATA_TYPE getArithmeticResultType(DATA_TYPE op1, DATA_TYPE op2);

/**
 * @brief Checks compatibility for relational operators (==, !=, <, >) and returns the result type.
 * @param op1 Type of the first operand.
 * @param op2 Type of the second operand.
 * @return Always BOOL_TYPE, because the result of a comparison is a Boolean.
 */
DATA_TYPE getRelationalResultType(DATA_TYPE op1, DATA_TYPE op2);

/**
 * @brief Checks compatibility for logical operators (&&, ||) and returns the result type.
 * @param op1 Type of the first operand.
 * @param op2 Type of the second operand.
 * @return Always BOOL_TYPE.
 */
DATA_TYPE getLogicalResultType(DATA_TYPE op1, DATA_TYPE op2);

/**
 * @brief Checks whether the expression in a conditional statement (if, while) is valid.
 * @param command The name of the command (“if,” “while”) for the error message.
 * @param type_expr The type of expression evaluated.
 */
void checkCondition(const char* command, DATA_TYPE type_expr);

/**
 * @brief Checks whether the type of a return expression is compatible with the declared return type of the function.
 * @param func_id The function identifier, obtained from the symbol table.
 * @param returned_expression_type The type of expression in the ‘return’ command.
 * @param tem_expr True if the ‘return’ statement has an expression, false otherwise (e.g., return;).
 */
void checkReturnFunction(IDENTIFIER func_id, DATA_TYPE returned_expression_type, bool tem_expr);

#endif // ANASEM_H