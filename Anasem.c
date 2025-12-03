#include "Anasem.h"

extern void erro(char *msg);

/**
 * @brief Reports a formatted semantic error and terminates compilation.
 * @param formato A printf-style format string.
 * @param ... Variable arguments for the format string.
 */
void erro_semantic(const char* formato, ...) {
    char buffer_msg[1024];
    va_list args;

    va_start(args, formato);
    vsnprintf(buffer_msg, sizeof(buffer_msg), formato, args);
    va_end(args);

    erro(buffer_msg);
}

/**
 * @brief Converts a value from the DATA_TYPE enum to its string representation.
 * @param type The type of data to be converted.
 * @return A constant string representing the type.
 */
const char* typeToString(DATA_TYPE type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_CHAR: return "char";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_VOID: return "void";
        default: return "desconhecido";
    }
}

/**
 * @brief Checks type compatibility in an assignment statement.
 * And allows some implicit conversions (e.g., int -> float).
 * @param type_lhs Left-hand type (variable).
 * @param type_rhs Right-hand type (expression).
 * @param nome_lhs Name of the variable on the left side, for error messages.
 */
void checkCompatibilityAssignment(DATA_TYPE type_lhs, DATA_TYPE type_rhs, const char* nome_lhs) {
    if (type_lhs == type_rhs) {
        return;
    }

    if ((type_lhs == TYPE_INT || type_lhs == TYPE_CHAR || type_lhs == TYPE_BOOL) &&
        (type_rhs == TYPE_INT || type_rhs == TYPE_CHAR || type_rhs == TYPE_BOOL)) {
        return; // Permitted conversion
    }
    if (type_lhs == TYPE_FLOAT && type_rhs == TYPE_INT) {
        return;
    }

    erro_semantic("Incompatible types in assignment. It is not possible to assign the type '%s' to variable '%s', which is of type '%s'.",
                     typeToString(type_rhs), nome_lhs, typeToString(type_lhs));
}

/**
 * @brief Checks type compatibility for arithmetic operators (+, -, *, /)
 * and returns the type resulting from the operation (with type promotion).
 * @param op1 Type of the first operand.
 * @param op2 Type of the second operand.
 * @return The resulting data type (e.g., FLOAT_TYPE if one of the operands is float).
 */
DATA_TYPE getArithmeticResultType(DATA_TYPE op1, DATA_TYPE op2) {
    
    if ((op1 == TYPE_INT || op1 == TYPE_CHAR || op1 == TYPE_FLOAT) &&
        (op2 == TYPE_INT || op2 == TYPE_CHAR || op2 == TYPE_FLOAT)) {
        
        if (op1 == TYPE_FLOAT || op2 == TYPE_FLOAT) {
            return TYPE_FLOAT;
        }

        if (op1 == TYPE_INT || op2 == TYPE_INT) {
            return TYPE_INT;
        }

        return TYPE_CHAR;
    }
    
    erro_semantic("Arithmetic operators require numeric types (int, char, float), but '%s' and '%s' were received.",
                   typeToString(op1), typeToString(op2));
    return UNKNOWN_TYPE; 
}

/**
 * @brief Checks compatibility for relational operators (==, !=, <, >) and returns the result type.
 * @param op1 Type of the first operand.
 * @param op2 Type of the second operand.
 * @return Always BOOL_TYPE, since the result of a comparison is a Boolean.
 */
DATA_TYPE getRelationalResultType(DATA_TYPE op1, DATA_TYPE op2) {
    if ((op1 == TYPE_INT || op1 == TYPE_CHAR || op1 == TYPE_FLOAT) &&
        (op2 == TYPE_INT || op2 == TYPE_CHAR || op2 == TYPE_FLOAT)) {
        return TYPE_BOOL; // The result of a relationship is always Boolean.
    }
    
    erro_semantic("Relational operators require numeric types (int, char, float), but '%s' and '%s' were received.",
                   typeToString(op1), typeToString(op2));
    return UNKNOWN_TYPE; 
}


/**
 * @brief Checks compatibility for logical operators (&&, ||) and returns the result type.
 * @param op1 Type of the first operand.
 * @param op2 Type do second operando.
 * @return Always BOOL_TYPE.
 */
DATA_TYPE getLogicalResultType(DATA_TYPE op1, DATA_TYPE op2) {
    if ((op1 == TYPE_BOOL || op1 == TYPE_INT) && (op2 == TYPE_BOOL || op2 == TYPE_INT)) {
        return TYPE_BOOL; // The result of a logical operation is always Boolean.
    }

    erro_semantic("Logical operators (&&, ||) require Boolean or int types, but '%s' and '%s' were received.",
                   typeToString(op1), typeToString(op2));
    return UNKNOWN_TYPE; // Should not be achieved
}


/**
 * @brief Checks whether the expression in a conditional statement (if, while) is valid.
 * @param comando The name of the command (“if,” “while”) for the error message.
 * @param tipo_expr The type of expression evaluated.
 */
void checkCondition(const char* comando, DATA_TYPE tipo_expr) {
    if (tipo_expr != TYPE_BOOL && tipo_expr != TYPE_INT) {
        erro_semantic("The conditional expression in a '%s' command must be of type boolean or int, but the type '%s' was received.",
                       comando, typeToString(tipo_expr));
    }
}

/**
 * @brief Checks whether the type of a return expression is compatible with the declared return type of the function.
 * @param func_id The function identifier, obtained from the symbol table.
 * @param tipo_expr_retornada The type of expression in the ‘return’ command.
 * @param tem_expr True if the ‘return’ statement has an expression, false otherwise (e.g., return;).
 */
void checkReturnFunction(IDENTIFIER func_id, DATA_TYPE tipo_expr_retornada, bool tem_expr) {
    DATA_TYPE tipo_retorno_declarado = func_id.type;

    if (tipo_retorno_declarado == TYPE_VOID) {
        if (tem_expr) {
            erro_semantic("The function '%s' is of type 'void' and cannot return a value.", func_id.name);
        }
    } else {
        if (!tem_expr) {
            erro_semantic("The function '%s' should return a value of type '%s', but no value was returned.",
                           func_id.name, typeToString(tipo_retorno_declarado));
        }
        
        checkCompatibilityAssignment(tipo_retorno_declarado, tipo_expr_retornada, func_id.name);
    }
}