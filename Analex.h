#ifndef _ANALEX_
#define _ANALEX_
#define MAX_LEXEME_SIZE 50
#define MAX_STRING_SIZE 100
#define NUM_SIZE 20 

#include <stdio.h>
#include <stdbool.h>

enum TOKEN_CAT {
    ID = 1,         // Identifier
    PR,             // Reserved Word
    SN,             // Signal
    CT_INT,         // Integer Constant
    CT_REAL,        // Real Constant
    CT_STRING,      // String Constant
    CT_CHAR,        // Character Constant
    FIN_EOF,        // End of File
};

enum SIGNS {
    ATRIB = 1,      // Attributes
    ADDIT,
    SUBTRACTT,
    MULTIPLICA,
    DIVISIO,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_SQUARE,
    CLOSE_SQUARE,
    OPEN_CURLY,
    CLOSE_CURLY,
    COMMA,
    SEMI_COLON,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_EQUAL,
    EQUALITY,
    DIFERENTE,
    NOT,
    AND,
    OR,
    E_COMERCIAL,
};

enum RESERVED_WORDS {
    INT, 
    CHAR,
    FLOAT,
    BOOL,
    VOID,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
};
typedef 
    struct {
        enum TOKEN_CAT cat; // Token category, receives a value of type enum TOKEN_CAT
        union { // To store different types of data
            int code; // Sign code or reserved word
            char lexema[MAX_LEXEME_SIZE]; // Character string that corresponds to the cat token. ID
            char string[MAX_STRING_SIZE]; // String for string constants in the cat token. STRINGCON
            char caractere; // Character char constants in the cat token. CHARCON
            int int_value; // Integer value for constants of type int, in the cat token. INTCON
            float real_value; // Actual value for constants of type float, in the cat token. REALCON
        };
    } TOKEN; // Definition of the TOKEN type

/* Global variables*/
extern int lineCont; // Line counter
extern int currLine; // Represents the current line
extern TOKEN t;
extern TOKEN tLookahead; // Token lookahead
extern FILE *fd; // Input file
extern char *reserved_words[];

/* Function signatures */
TOKEN Analex(FILE *fd);
TOKEN AnalexTLA(FILE *fd);
void erro(char *msg);
void printToken(TOKEN token);

#endif