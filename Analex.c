#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Analex.h"

// Global Definitions and Variables
int lineCont = 1;
int currLine = 1;
TOKEN t;
TOKEN tLookahead; // Token lookahead
FILE *fd;

// Reserved Words
char *reserved_words[] = {
    "int", "char", "float", "bool", "void",
    "if", "else", "while", "for", "return"
};
const int NUMBER_OF_RESERVED_WORDS = sizeof(reserved_words) / sizeof(reserved_words[0]);

// Escape Character Constants 
#define BARRA_N '\n'
#define BARRA_0 '\0'

void erro(char *msg) {
    fprintf(stderr, "ERROR (Line %d): %s\n", currLine, msg);
    exit(1);
}

TOKEN Analex(FILE *input_file) {
    TOKEN t;
    static bool initializer = true;
    currLine = lineCont;

    if (fd == NULL) {
        fd = input_file;
    }

    if (initializer) {
        initializer = false;
        t = AnalexTLA(fd);
        if (t.cat != FIN_EOF) {
            tLookahead = AnalexTLA(fd);
        } else {
            tLookahead = t;
        }


        //printToken(t);
        return t;
    }
    else if (tLookahead.cat == FIN_EOF) {
        t = tLookahead;
        return t;
    }
    else {
        t = tLookahead;
        tLookahead = AnalexTLA(fd);
        //printToken(t);
        return t;
    }
}

TOKEN AnalexTLA(FILE *input_file) {
    if (fd == NULL) {
        fd = input_file;
    }

    int state = 0;
    // Separate Buffers
    char lexema[MAX_LEXEME_SIZE];
    char digitos[NUM_SIZE];
    char string[MAX_STRING_SIZE];
    int tamL = 0, tamD = 0, tamS = 0;

    while (true) {
        char c = fgetc(fd);

        switch (state) {
            // State q0 Initial
            case 0:
                tamL = 0; tamD = 0; tamS = 0;
                if (c == ' ' || c == '\t') { state = 0; }
                else if (c == '\n') { lineCont++; state = 0; }
                else if (isalpha(c) || c == '_') {
                    lexema[tamL++] = c;
                    state = 1;
                }
                else if (isdigit(c)) {
                    digitos[tamD++] = c;
                    state = 3;
                }
                else if (c == '\"') {
                    state = 9;
                }
                else if (c == '\'') {
                    state = 8;
                }
                else if (c == '/') {
                    state = 16;
                }
                else if (c == '<') {
                    state = 10;
                }
                else if (c == '>') {
                    state = 11;
                }
                else if (c == '=') {
                    state = 12;
                }
                else if (c == '!') {
                    state = 13;
                }
                else if (c == '&') {
                    state = 14;
                }
                else if (c == '|') {
                    state = 15;
                }
                else if (c == EOF) {
                    state = 49;
                    t.cat = FIN_EOF;
                    return t;
                }
                else if (c == '*') {
                    state = 41;
                    t.cat = SN;
                    t.code = MULTIPLICA;
                    return t;
                }
                else if (c == '-') {
                    state = 39;
                    t.cat = SN;
                    t.code = SUBTRACTT;
                    return t;
                }
                else if (c == '+') {
                    state = 40;
                    t.cat = SN;
                    t.code = ADDIT;
                    return t;
                }
                else if (c == '(') {
                    state = 42;
                    t.cat = SN;
                    t.code = OPEN_PAREN;
                    return t;
                }
                else if (c == ')') {
                    state = 43;
                    t.cat = SN;
                    t.code = CLOSE_PAREN;
                    return t;
                }
                else if (c == '[') {
                    state = 44;
                    t.cat = SN;
                    t.code = OPEN_SQUARE;
                    return t;
                }
                else if (c == ']') {
                    state = 45;
                    t.cat = SN;
                    t.code = CLOSE_SQUARE;
                    return t;
                }
                else if (c == '{') {
                    state = 46;
                    t.cat = SN;
                    t.code = OPEN_CURLY;
                    return t;
                }
                else if (c == '}') {
                    state = 47;
                    t.cat = SN;
                    t.code = CLOSE_CURLY;
                    return t;
                }
                else if (c == ';') {
                    state = 50;
                    t.cat = SN;
                    t.code = SEMI_COLON;
                    return t;
                }
                else if (c == ',') {
                    state = 48;
                    t.cat = SN;
                    t.code = COMMA;
                    return t;
                }
                else {
                    erro("Invalid character.");
                }
                break;

            // case 1 assignment to state q2
            case 1:
                if (isalnum(c) || c == '_') {
                    lexema[tamL++] = c;
                    state = 1; // remain in state q1 which is non-state
                } else {
                    ungetc(c, fd);
                    lexema[tamL] = '\0';
                    
                    state = 2; // assignment for state q2

                    // reserved word check here // NOTE: PR means reserved word
                    bool eh_pr = false;
                    for (int i = 0; i < NUMBER_OF_RESERVED_WORDS; i++) {
                        if (strcmp(lexema, reserved_words[i]) == 0) {
                            t.cat = PR; t.code = i; eh_pr = true;
                            return t;
                        }
                    }
                    
                    // if not reserved word then it's ID
                    t.cat = ID; 
                    strcpy(t.lexema, lexema);
                    return t;
                }
                break;

            // states q3, q5, q6
            case 3:
                if (isdigit(c)) { digitos[tamD++] = c; state = 3; }
                else if (c == '.') { digitos[tamD++] = c; state = 5; } // go to q5
                else {
                    ungetc(c, fd);
                    digitos[tamD] = '\0';
                    state = 4; // final state q4
                    t.cat = CT_INT; t.int_value = atoi(digitos);
                    return t;
                }
                break;
            case 5:
                if (isdigit(c)) { digitos[tamD++] = c; state = 6; } // go to q6
                else { erro("Malformed real constant."); }
                break;
            case 6:
                if (isdigit(c)) { digitos[tamD++] = c; state = 6; }
                else {
                    ungetc(c, fd);
                    digitos[tamD] = '\0';
                    state = 7; // final state q7
                    t.cat = CT_REAL; t.real_value = atof(digitos);
                    return t;
                }
                break;
            
            // character logic for q8, q32, q33, q34, q35, q36
            case 8:
                if (c == '\\') {
                    state = 33;
                }
                else if (c != '\'') {
                    lexema[0] = c;
                    state = 32;
                }
                else {
                    erro("Empty character constant.");
                }
                break;

            case 32:
                if (c == '\'') {
                    state = 34;
                    t.cat = CT_CHAR;
                    t.caractere = lexema[0];
                    return t;
                }
                else {
                    erro("Constant character with more than one character.");
                }
                break;

            case 33:
                if (c == 'n') {
                    lexema[0] = BARRA_N;
                    state = 35;
                }
                else if (c == '0') {
                    lexema[0] = BARRA_0;
                    state = 35;
                }
                else {
                    erro("Invalid escape sequence.");
                }
                break;

            case 35:
                if (c == '\'') {
                    state = 36;
                    t.cat = CT_CHAR;
                    t.caractere = lexema[0];
                    return t;
                }
                else {
                    erro("Malformed escape character constant.");
                }
                break;

            // string logic state q9, q31
            case 9:
                if (c == '\"') {
                    string[tamS] = '\0';
                    state = 31; // final state q31
                    t.cat = CT_STRING;
                    strcpy(t.string, string);
                    return t;
                }
                else if (c == '\n' || c == EOF) {
                    erro("Unfinished string.");
                }
                else {
                    string[tamS++] = c;
                    state = 9;
                }
                break;

            // states containing multiple character operators
            case 10:
                if (c == '=') {
                    state = 17;
                    t.cat = SN;
                    t.code = LESS_THAN_OR_EQUAL;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    state = 18;
                    t.cat = SN;
                    t.code = LESS_THAN;
                    return t;
                }

            case 11:
                if (c == '=') {
                    state = 19;
                    t.cat = SN;
                    t.code = GREATER_EQUAL;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    state = 20;
                    t.cat = SN;
                    t.code = GREATER_THAN;
                    return t;
                }

            case 12:
                if (c == '=') {
                    state = 21;
                    t.cat = SN;
                    t.code = EQUALITY;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    state = 22;
                    t.cat = SN;
                    t.code = ATRIB;
                    return t;
                }

            case 13:
                if (c == '=') {
                    state = 23;
                    t.cat = SN;
                    t.code = DIFERENTE;
                    return t;
                }
                else {
                    ungetc(c, fd); // return character to stream
                    state = 24;
                    t.cat = SN;
                    t.code = NOT;
                    return t;
                }

            case 14:
                if (c == '&') {
                    state = 25;
                    t.cat = SN;
                    t.code = AND;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    state = 26;
                    t.cat = SN;
                    t.code = E_COMERCIAL;
                    return t;
                }

            case 15:
                if (c == '|') {
                    state = 27;
                    t.cat = SN;
                    t.code = OR;
                    return t;
                }
                else {
                    erro("Invalid operator '|', expected '||'.");
                }
                break; // break for cases that do not return

            // states for comment and division
            case 16:
                if (c == '*') {
                    state = 28;
                }
                else {
                    ungetc(c, fd);
                    state = 29;
                    t.cat = SN;
                    t.code = DIVISIO;
                    return t;
                }
                break;

            case 28: // inside comments
                if (c == '*') {
                    state = 30;
                }
                else if (c == EOF) {
                    erro("Unfinished comment.");
                }
                else {
                    state = 28;
                }
                break;

            case 30: // if '*' in comment
                if (c == '/') {
                    state = 0;
                }
                else if (c == EOF) {
                    erro("Unfinished comment.");
                }
                else if (c != '*') {
                    state = 28;
                }
                else {
                    state = 30; // if '***'
                }
                break;

        } // end of switch
    } // end of while
}

// =================================================================================
// Auxiliary Function for Printing Signs (Operators)
const char* getSinalStr(int codigo_sinal) {
    switch(codigo_sinal) {
        case ATRIB: return "=";
        case ADDIT: return "+";
        case SUBTRACTT: return "-";
        case MULTIPLICA: return "*";
        case DIVISIO: return "/";
        case OPEN_PAREN: return "(";
        case CLOSE_PAREN: return ")";
        case OPEN_SQUARE: return "[";
        case CLOSE_SQUARE: return "]";
        case OPEN_CURLY: return "{";
        case CLOSE_CURLY: return "}";
        case COMMA: return ",";
        case SEMI_COLON: return ";";
        case LESS_THAN: return "<";
        case GREATER_THAN: return ">";
        case LESS_THAN_OR_EQUAL: return "<=";
        case GREATER_EQUAL: return ">=";
        case EQUALITY: return "==";
        case DIFERENTE: return "!=";
        case NOT: return "!";
        case AND: return "&&";
        case OR: return "||";
        case E_COMERCIAL: return "&";
        default: return "UNKNOWN_SIGNAL";
    }
}

void printToken(TOKEN token) {
     switch (token.cat) {
            case ID:
                printf("Line %-3d: <ID, %s>\n", currLine, token.lexema);
                break;
            case PR:
                printf("Line %-3d: <PR, %s>\n", currLine, reserved_words[token.code]);
                break;
            case CT_INT:
                printf("Line %-3d: <CT_INT, %d>\n", currLine, token.int_value);
                break;
            case CT_REAL:
                printf("Line %-3d: <CT_REAL, %f>\n", currLine, token.real_value);
                break;
            case CT_STRING:
                printf("Line %-3d: <CT_STRING, \"%s\">\n", currLine, token.string);
                break;
            case CT_CHAR:
                if (token.caractere == '\n') {
                    printf("Line %-3d: <CT_CHAR, '\\n'>\n", currLine);
                } else if (t.caractere == '\0') {
                    printf("Line %-3d: <CT_CHAR, '\\0'>\n", currLine);
                } else {
                    printf("Line %-3d: <CT_CHAR, '%c'>\n", currLine, token.caractere);
                }
                break;
            case SN:
                printf("Line %-3d: <SN, %s>\n", currLine, getSinalStr(token.code));
                break;
            case FIN_EOF:
                printf("--- EOF ---\n");
        }
}