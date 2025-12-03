/*
  This file will contain the implementation of the Recursive Descent Parser (ASDR). 
  Each function corresponds to a non-terminal of the grammar below.

======================================================================
Production Rules of the Cshort Grammar - Without Backtracking 
======================================================================

<prog>        ::= { decl ( ';' | '{' body_func '}' ) }

<decl>        ::= <type> <decl_var> { ',' <decl_var> }
              | <type> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }
              | <void> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }

<decl_var>    ::= <id> [ '[' <intcon> ']' ]

<type>        ::= 'char' | 'int' | 'float' | 'bool'

<tipos_param> ::= <void>
              | <type> [ '&' ] <id> [ '[' ']' ] { ',' <type> [ '&' ] <id> [ '[' ']' ] }

<body_func>  ::= { type decl_var { ',' decl_var} ';' } { cmd }

// Command Rules
<cmd>         ::= 'if' '(' expr ')' cmd [ 'else' cmd ]
              | 'while' '(' expr ')' cmd
              | 'for' '(' [atrib] ';' [expr] ';' [atrib] ')' cmd
              | 'return' [expr] ';'
              | '{' {cmd} '}'
              | id cmd_cont

<cmd_cont>    ::= '(' [expr {',' expr}] ')' ';'
              | ['[' expr ']'] '=' expr ';'


<atrib>       ::= <id> [ '[' <expr> ']' ] '=' <expr>

<expr>        ::= <expr_simp> [ <op_rel> <expr_simp> ]

<expr_simp>   ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

<termo>       ::= <fator> { ( '*' | '/' | '&&' ) <fator> }

// Factor Rules (factored)
fator         ::= intcon
              | realcon
              | charcon
              | '(' expr ')'
              | '!' fator
              | id factor_cont

factor_cont    ::= '[' expr ']'
              | '(' [expr {',' expr}] ')'
              | ε

<op_rel>      ::= '==' | '!=' | '<=' | '<' | '>=' | '>'

*/

#include "Anasint.h" 


SCOPE scope = GLOBAL;
IDENTIFIER current_function;
int offset_local_atual = 0;
int offset_global_atual = 0;


void process_list_declaration_vars(DATA_TYPE type) {
    TOKEN idToken;

    if (t.cat != ID) {
        erro("Expected identifier in variable declaration.");
    }
    idToken = t;

    //Check by redeclaration within the current scope.
    if (SearchTableIDSameScope(idToken.lexema, scope) != -1) {
        char msg[100];
        sprintf(msg, "Re-declaration of the identifier '%s'", idToken.lexema);
        erro(msg);
    }

    t = Analex(fd); // Consumes the ID

    if (t.cat == SN && t.code == OPEN_SQUARE) {
        // It's an array

        t = Analex(fd); // Consumes '['
        if (t.cat != CT_INT) erro("Array size must be an integer constant.");
        int tamArray = t.int_value;

        InsertTableID(idToken.lexema, CAT_VAR, scope, type, false, true, tamArray, offset_local_atual++);

        t = Analex(fd); // Consumes the number
        if (t.cat != SN || t.code != CLOSE_SQUARE) erro("Expected ']' to close the array dimension.");
        t = Analex(fd); // Consumes ']'

    } else {
        // It's a simple variable
        InsertTableID(idToken.lexema, CAT_VAR, scope, type, false, false, 0, offset_local_atual++);
    }
}


DATA_TYPE getTokenType(int cod) {
    switch(cod) {
        case INT: return TYPE_INT;
        case CHAR: return TYPE_CHAR;
        case FLOAT: return TYPE_FLOAT;
        case BOOL: return TYPE_BOOL;
        default: return TYPE_VOID;
    }
}

int contLabel = 0;
int newRotulo() {
    return contLabel++;
}


void Prog() {
    DECL_FLAG declFlag;
 
    GenerateCode("INIP");
    t = Analex(fd);
 
    while (t.cat != FIN_EOF) {
        declFlag = Decl();
        if (t.cat == SN && t.code == SEMI_COLON) {
            t = Analex(fd); 
        }
        else if (t.cat == SN && t.code == OPEN_CURLY) {

            current_function = tableIdentifiers.identifiers[tableIdentifiers.tamTabela - 1];

            // Generates the function header
            GenerateCode("LABEL %s", current_function.name);
            GenerateCode("INIPR 1");

            t = Analex(fd); // Consumes the '{'
            int locais_count = body_func();
            
            if (!(t.cat == SN && t.code == CLOSE_CURLY)) {
                erro("Expected '}' to close the function body");
            }
            
            if (locais_count > 0) {
                GenerateCode("DMEM %d", locais_count);
            }
            GenerateCode("RET 1, 0");

            t = Analex(fd); // Consumes the '}'
        }
        else {
            erro("Expected ';' or '{' after the declaration");
        }
    }
    
    int main_idx = SearchTableID("main");
    if (main_idx == -1) {
        erro("Function 'main' not defined in the program.");
    }
    
    GenerateCode("CALL main");
    GenerateCode("HALT");
}


void process_array() {
    t = Analex(fd);
    if (t.cat != CT_INT) {
        erro("Array size must be an integer constant.");
    }
    t = Analex(fd);
    if (t.cat != SN || t.code != CLOSE_SQUARE) {
        erro("Expected ']' to close the array dimension.");
    }
}

DECL_FLAG Decl() {
    DECL_FLAG declFlag = NO_DECL;
    TOKEN idToken;
    DATA_TYPE typeData;
    int type;
    int contProt = 0;
 
    if (t.cat == PR && (t.code == INT || t.code == FLOAT || t.code == BOOL || t.code == CHAR || t.code == VOID)) {
        type = t.code;
        typeData = getTokenType(type);
        t = Analex(fd); 
 
        if (t.cat != ID) {
            erro("Identifier expected");
        }
        idToken = t; 
 
        if (SearchTableIDSameScope(idToken.lexema, scope) != -1) {
            char msg[100];
            sprintf(msg, "Redeclaration of identifier '%s'", idToken.lexema);
            erro(msg);
        }

        if (tLookahead.cat == SN && tLookahead.code == OPEN_PAREN) {
            contProt = 1;
            declFlag = DECL_PROT;
            InsertTableID(idToken.lexema, CAT_FUNC, scope, typeData, false, false, 0, 0);
            t = Analex(fd);
            scope = LOCAL;
            t = Analex(fd);
            Type_param();
            if (t.cat != SN || t.code != CLOSE_PAREN) {
                erro("Expected ')' to close the function parameters");
            }
            t = Analex(fd);
            scope = GLOBAL;
             if (contProt == 1) {
                declFlag = DECL_PROT_UNICO; 
            }
 
        } else { 
            declFlag = DECL_VAR;
            if (type == VOID) {
                erro("Variable cannot be of type void");
            }
 
            t = Analex(fd); 
            if (t.cat == SN && t.code == OPEN_SQUARE) {
                t = Analex(fd); 
                if (t.cat != CT_INT) erro("Array size must be an integer constant.");
                int tamArray = t.int_value;
                InsertTableID(idToken.lexema, CAT_VAR, scope, typeData, false, true, tamArray, offset_global_atual++);
                t = Analex(fd); 
                if (t.cat != SN || t.code != CLOSE_SQUARE) erro("Expected ']' to close the array dimension.");
                t = Analex(fd); 
            } else {
                InsertTableID(idToken.lexema, CAT_VAR, scope, typeData, false, false, 0, offset_global_atual++);
            }

            while (t.cat == SN && t.code == COMMA) {
                t = Analex(fd); 
                if (t.cat != ID) erro("Identifier expected after comma");
                idToken = t;
                if (SearchTableIDSameScope(idToken.lexema, scope) != -1) {
                    char msg[100];
                    sprintf(msg, "Redeclaration of identifier '%s'", idToken.lexema);
                    erro(msg);
                }
                t = Analex(fd); 
                if (t.cat == SN && t.code == OPEN_SQUARE) {
                     t = Analex(fd); 
                    if (t.cat != CT_INT) erro("Array size must be an integer constant.");
                    int tamArray = t.int_value;
                    InsertTableID(idToken.lexema, CAT_VAR, scope, typeData, false, true, tamArray, offset_global_atual++);
                    t = Analex(fd); 
                    if (t.cat != SN || t.code != CLOSE_SQUARE) erro("Expected ']' to close the array dimension.");
                    t = Analex(fd); 
                } else {
                    InsertTableID(idToken.lexema, CAT_VAR, scope, typeData, false, false, 0, offset_global_atual++);
                }
            }
        }
 
    } else {
        erro("Invalid or missing type in declaration");
    }
    return declFlag;
}

int body_func() {
    int var_locais_count = 0;
    offset_local_atual = 0;
    
    scope = LOCAL;

    
    while (t.cat == PR && (t.code == INT || t.code == CHAR || t.code == FLOAT || t.code == BOOL)) {
        DATA_TYPE tipo_atual = getTokenType(t.code);
        t = Analex(fd);

        process_list_declaration_vars(tipo_atual);
        var_locais_count++;

        while(t.cat == SN && t.code == COMMA) { 
            t = Analex(fd);
            process_list_declaration_vars(tipo_atual);
            var_locais_count++;
        }
        
        if (t.cat != SN || t.code != SEMI_COLON) {
            erro("Expected ';' at the end of the local declaration.");
        }
        t = Analex(fd);
    }

    if (var_locais_count > 0) {
        GenerateCode("AMEM %d", var_locais_count);
    }

    while(t.cat != SN || t.code != CLOSE_CURLY) {
        Cmd();
    }

    scope = GLOBAL;
    return var_locais_count;
}


void Decl_var() {
    if (t.cat != ID) {
        erro("Expected identifier in variable declaration.");
    }
    t = Analex(fd);
    if (t.cat == SN && t.code == OPEN_SQUARE) {
        t = Analex(fd);
        if (t.cat != CT_INT) {
            erro("Array size must be an integer constant.");
        }
        t = Analex(fd);
        if (t.cat != SN || t.code != CLOSE_SQUARE) {
            erro("Expected ']' to close the array dimension.");
        }
        t = Analex(fd);
    }
}


void Type() {
    if (t.cat == PR && (t.code == INT || t.code == CHAR || t.code == FLOAT || t.code == BOOL)) {
        t = Analex(fd);
    } else {
        erro("Expected a type (int, char, float, bool).");
    }
}

void Type_param() {
    if (t.cat == PR && t.code == VOID) {
        t = Analex(fd);
        return;
    }
    else {
        Type();
        if (t.cat == SN && t.code == E_COMERCIAL) {
            t = Analex(fd);
        }
        if (t.cat != ID) erro("Expected parameter identifier.");
        t = Analex(fd);
        if (t.cat == SN && t.code == OPEN_SQUARE) {
            t = Analex(fd);
            if (t.cat != SN || t.code != CLOSE_SQUARE) erro("Expected ']' in array parameter.");
            t = Analex(fd);
        }
        while(t.cat == SN && t.code == COMMA) {
            t = Analex(fd);
            Type();
            if (t.cat == SN && t.code == E_COMERCIAL) {
                t = Analex(fd);
            }
            if (t.cat != ID) erro("Expected parameter identifier.");
            t = Analex(fd);
            if (t.cat == SN && t.code == OPEN_SQUARE) {
                t = Analex(fd);
                if (t.cat != SN || t.code != CLOSE_SQUARE) erro("Expected ']' in array parameter.");
                t = Analex(fd);
            }
        }
    }
}



void cmd_cont(TOKEN id_alvo) {
    if (t.cat == SN && t.code == OPEN_PAREN) {
        t = Analex(fd);
        if (t.cat != SN || t.code != CLOSE_PAREN) {
            Expr(); 
            while(t.cat == SN && t.code == COMMA) {
                t = Analex(fd);
                Expr();
            }
        }
        if (t.cat != SN || t.code != CLOSE_PAREN) {
            erro("Expected ')' to close function call.");
        }
        t = Analex(fd);
        if (t.cat != SN || t.code != SEMI_COLON) {
            erro("Expected ';' after function call.");
        }
        t = Analex(fd);
    }
    else {
        
        int idx = SearchTableID(id_alvo.lexema);
        if (idx == -1) {
            char msg[100];
            sprintf(msg, "Variable '%s' not declared.", id_alvo.lexema);
            erro(msg);
        }

        IDENTIFIER id = tableIdentifiers.identifiers[idx];
        DATA_TYPE tipo_lhs = id.type;
        
        if (t.cat == SN && t.code == OPEN_SQUARE) {
            if (!id.array) {
                char msg[100];
                sprintf(msg, "Variable '%s' is not an array.", id.name);
                erro(msg);
            }
            t = Analex(fd);

            DATA_TYPE tipo_indice = Expr();
            if (tipo_indice != TYPE_INT) {
                erro("Array index must be an expression of type int.");
            }

            if (t.cat != SN || t.code != CLOSE_SQUARE) {
                erro("Expected ']' in array assignment.");
            }
            t = Analex(fd);
        }

        if (t.cat != SN || t.code != ATRIB) {
            erro("Expected '=' in assignment statement.");
        }
        t = Analex(fd); // Consumes '='
        
        DATA_TYPE tipo_rhs = Expr();
        
        checkCompatibilityAssignment(tipo_lhs, tipo_rhs, id.name);
        
        id = tableIdentifiers.identifiers[idx];
        GenerateCode("STOR %s", id.name);

        
        if (t.cat != SN || t.code != SEMI_COLON) {
            erro("Expected ';' after assignment statement.");
        }
        t = Analex(fd); // Consome ';'
    }
}


void Atrib() {
    TOKEN id_alvo;
    if (t.cat != ID) {
        erro("Expected identifier in assignment statement.");
    }
    id_alvo = t;
    t = Analex(fd);
    if (t.cat == SN && t.code == OPEN_SQUARE) {
        t = Analex(fd);
        Expr();
        if (t.cat != SN || t.code != CLOSE_SQUARE) {
            erro("Expected ']' in array assignment.");
        }
        t = Analex(fd);
    }
    if (t.cat != SN || t.code != ATRIB) {
        erro("Expected '=' in assignment statement.");
    }
    t = Analex(fd);
    Expr();
    printf("STOR %s", id_alvo.lexema);
}


void Cmd() {
    if (t.cat == PR && t.code == IF) {
        int rotuloElse, rotuloFim;
        t = Analex(fd); 
        if (t.cat != SN || t.code != OPEN_PAREN) erro("Expected '(' after 'if'.");
        t = Analex(fd); 
        
        DATA_TYPE tipo_expr_if = Expr(); 
        checkCondition("if", tipo_expr_if); 
        
        if (t.cat != SN || t.code != CLOSE_PAREN) erro("Expected ')' after expression in 'if'.");
        t = Analex(fd); 
 
        rotuloElse = newRotulo();
        GenerateCode("GOFALSE L%d", rotuloElse);
        Cmd(); 

        if (t.cat == PR && t.code == ELSE) {
            rotuloFim = newRotulo();
            GenerateCode("GOTO L%d", rotuloFim);
            GenerateCode("LABEL L%d", rotuloElse);
            t = Analex(fd); 
            Cmd(); 
            GenerateCode("LABEL L%d", rotuloFim);
        } else {
            GenerateCode("LABEL L%d", rotuloElse);
        }
    }
    else if (t.cat == PR && t.code == WHILE) {
        int rotuloInicio, rotuloFim;
        rotuloInicio = newRotulo();
        rotuloFim = newRotulo();
        GenerateCode("LABEL L%d", rotuloInicio);
        t = Analex(fd); 
        if (t.cat != SN || t.code != OPEN_PAREN) erro("Expected '(' after 'while'.");
        t = Analex(fd); 
        
        DATA_TYPE tipo_expr_while = Expr();
        checkCondition("while", tipo_expr_while);
        
        if (t.cat != SN || t.code != CLOSE_PAREN) erro("Expected ')' after expression in 'while'.");
        t = Analex(fd);
        GenerateCode("GOFALSE L%d", rotuloFim);
        Cmd(); 

        GenerateCode("GOTO L%d", rotuloInicio);
        GenerateCode("LABEL L%d", rotuloFim);
    }
    else if (t.cat == PR && t.code == FOR) {
        t = Analex(fd); if (t.cat != SN || t.code != OPEN_PAREN) erro("Expected '(' after 'for'.");
        t = Analex(fd); 
        if (t.cat != SN || t.code != SEMI_COLON) {
            Atrib();
        } else {
            t = Analex(fd); 
        }
        
        if (t.cat != SN || t.code != SEMI_COLON) {
            DATA_TYPE tipo_expr_for = Expr(); 
            checkCondition("for", tipo_expr_for);
        }
        if (t.cat != SN || t.code != SEMI_COLON) erro("Expected ';' in the 2nd part of 'for'.");
        t = Analex(fd); 
        if (t.cat != SN || t.code != CLOSE_PAREN) Atrib();
        if (t.cat != SN || t.code != CLOSE_PAREN) erro("Expected ')' to close the 'for'.");
        t = Analex(fd); 
        Cmd();
    }
    else if (t.cat == PR && t.code == RETURN) {
        t = Analex(fd);
        
        DATA_TYPE tipo_retorno = TYPE_VOID;
        bool tem_expressao = (t.cat != SN || t.code != SEMI_COLON);
        
        if (tem_expressao) {
            tipo_retorno = Expr(); 
        }
        
        checkReturnFunction(current_function, tipo_retorno, tem_expressao);
        
        if (t.cat != SN || t.code != SEMI_COLON) erro("Expected ';' after 'return'.");
        t = Analex(fd);
    }
    else if (t.cat == SN && t.code == OPEN_CURLY) {
        t = Analex(fd);
        while(t.cat != SN || t.code != CLOSE_CURLY) { 
            Cmd(); 
        }
        t = Analex(fd);
    }
    else if (t.cat == ID) {
        TOKEN id_token = t;
        t = Analex(fd);
        cmd_cont(id_token);
    }
    else {
        erro("Invalid command.");
    }
}


DATA_TYPE Termo() {
    DATA_TYPE tipo_esq = Fator(); 
    
    while (t.cat == SN && (t.code == MULTIPLICA || t.code == DIVISIO || t.code == AND )) {
        TOKEN op = t;
        t = Analex(fd);
        DATA_TYPE tipo_dir = Fator(); 
        
        if (op.code == MULTIPLICA || op.code == DIVISIO) {
            tipo_esq = getArithmeticResultType(tipo_esq, tipo_dir);
            if (op.code == MULTIPLICA) GenerateCode("MUL");
            else if (op.code == DIVISIO) GenerateCode("DIV");

        } else if (op.code == AND) {
            tipo_esq = getLogicalResultType(tipo_esq, tipo_dir);

            GenerateCode("AND");
        }
    }
    return tipo_esq; 
}

DATA_TYPE Expr_simp() {
    if (t.cat == SN && (t.code == ADDIT || t.code == SUBTRACTT)) {
        t = Analex(fd);
    }
    
    DATA_TYPE tipo_esq = Termo();
    
    while (t.cat == SN && (t.code == ADDIT || t.code == SUBTRACTT || t.code == OR)) {
        TOKEN op = t;
        t = Analex(fd);
        DATA_TYPE tipo_dir = Termo();

        if (op.code == ADDIT || op.code == SUBTRACTT) {
            tipo_esq = getArithmeticResultType(tipo_esq, tipo_dir);
            if (op.code == ADDIT) GenerateCode("ADD"); 
            else if (op.code == SUBTRACTT) GenerateCode("SUB"); 

        } else if (op.code == OR) {
            tipo_esq = getLogicalResultType(tipo_esq, tipo_dir);
            GenerateCode("OR");
        }
    }
    return tipo_esq; 
}

    
DATA_TYPE Expr() {
    DATA_TYPE tipo_esq = Expr_simp(); 

    if (isOp_rel(t)) {
        TOKEN op = t;
        Op_rel(); // Consumes the relational operator
        DATA_TYPE tipo_dir = Expr_simp(); 

        DATA_TYPE tipo_resultado = getRelationalResultType(tipo_esq, tipo_dir);
        
        GenerateCode("SUB"); 
        
        switch (op.code) {
            case EQUALITY:
                // If (a-b) == 0, then a == b.
                GenerateCode("EQ");
                break;
            case DIFERENTE:
                // If (a-b) != 0, then a != b.
                GenerateCode("NE");
                break;
            case LESS_THAN:
                // If (a-b) < 0, then a < b.
                GenerateCode("LT");
                break;
            case LESS_THAN_OR_EQUAL:
                // If (a-b) <= 0, then a <= b.
                GenerateCode("LE");
                break;
            case GREATER_THAN:
                // If (a-b) > 0, then a > b.
                GenerateCode("GT");
                break;
            case GREATER_EQUAL:
                // If (a-b) >= 0, then a >= b.
                GenerateCode("GE");
                break;
        }
        
        return tipo_resultado; // The type of the expression is always boolean
    }
    return tipo_esq; 
}


void factor_cont(IDENTIFIER func_id) {
    t = Analex(fd); // Consumes the '('

    int indice_primeiro_param = func_id.address + 1;
    int arg_count = 0;

    if (t.cat != SN || t.code != CLOSE_PAREN) {
        // Processes the first argument
        arg_count++;
        DATA_TYPE tipo_arg = Expr();
        
        // Validation of the first argument
        if (tableIdentifiers.identifiers[indice_primeiro_param].categoria != CAT_PARAM) {
            erro_semantic("Function '%s' does not expect arguments.", func_id.name);
        }
        DATA_TYPE tipo_param_esperado = tableIdentifiers.identifiers[indice_primeiro_param].type;
        checkCompatibilityAssignment(tipo_param_esperado, tipo_arg, "function argument");

        // Loop to process the other arguments
        while(t.cat == SN && t.code == COMMA) {
            t = Analex(fd);
            arg_count++;
            tipo_arg = Expr();

            // Validation of the subsequent arguments
            int indice_param_atual = indice_primeiro_param + arg_count - 1;
             if (tableIdentifiers.identifiers[indice_param_atual].categoria != CAT_PARAM) {
                erro_semantic("Excess arguments for function '%s'.", func_id.name);
            }
            tipo_param_esperado = tableIdentifiers.identifiers[indice_param_atual].type;
            checkCompatibilityAssignment(tipo_param_esperado, tipo_arg, "function argument");
        }
    }

    // TODO: Add final check to see if the number of arguments matches the number of parameters.
    if (t.cat != SN || t.code != CLOSE_PAREN) {
        erro("Expected ')' to close function call.");
    }
    t = Analex(fd); // Consumes the ')'
}


void fator_cont_array(IDENTIFIER id) {
    if (t.cat != SN || t.code != OPEN_SQUARE) {
        return;
    }
    
    if (!id.array) {
        erro_semantic("Variable '%s' is not an array and cannot be indexed.", id.name);
    }
    
    t = Analex(fd); // Consumes the '['
    
    DATA_TYPE tipo_indice = Expr();
    
    if (tipo_indice != TYPE_INT) {
        erro_semantic("The index of an array must be an expression of type int.");
    }
    
    GenerateCode("ADD");
    
    GenerateCode("LDSTK %d", id.scope == GLOBAL ? 0 : 1);
    
    if (t.cat != SN || t.code != CLOSE_SQUARE) {
        erro("Expected ']' to close array access.");
    }
    t = Analex(fd); // Consumes the ']'
}


DATA_TYPE Fator() {
    DATA_TYPE tipoRetorno = TYPE_VOID;

    if (t.cat == CT_INT) {
        GenerateCode("PUSH %d", t.int_value);
        t = Analex(fd); 
        return TYPE_INT;
    } 
    else if (t.cat == CT_REAL) {
        GenerateCode("PUSH %f", t.real_value);
        t = Analex(fd);
        return TYPE_FLOAT; 
    }
    else if (t.cat == CT_CHAR) {
        GenerateCode("PUSH %d", (int)t.caractere);
        t = Analex(fd);
        return TYPE_CHAR; 
    }
    else if (t.cat == SN && t.code == OPEN_PAREN) {
        t = Analex(fd); 
        tipoRetorno = Expr(); 
        if (t.cat != SN || t.code != CLOSE_PAREN) {
            erro("Expected ')' after expression in factor.");
        }
        t = Analex(fd);
        return tipoRetorno;
    }
    else if (t.cat == SN && t.code == NOT) {
        t = Analex(fd);
        Fator();
        return TYPE_BOOL;
    }
    else if (t.cat == ID) {
        TOKEN idToken = t;
        int idx = SearchTableID(idToken.lexema);
        if (idx == -1) {
            erro("ERROR: Identifier not found.");
        }
        
        IDENTIFIER id = tableIdentifiers.identifiers[idx];
        
        t = Analex(fd); // Consumes the ID
        
        if (t.cat == SN && t.code == OPEN_PAREN) {
            if (id.categoria != CAT_FUNC) {
                 erro_semantic("Identifier '%s' is not a function.", id.name);
            }
            factor_cont(id);
            return id.type;
        } 
        // If it's not a function, it's a variable.
        else {
             if (id.categoria != CAT_VAR) {
                 erro_semantic("Invalid use of function identifier '%s' as a variable.", id.name);
            }
            
            if (id.array) {
                GenerateCode("PUSH %d", id.address);
            } else {
                GenerateCode("LOAD %d, %d", id.scope == GLOBAL ? 0 : 1, id.address);
            }

            fator_cont_array(id);

            return id.type;
        }
    }
    else {
        erro("Invalid factor: expected constant, id, '!' or '('");
    }
}


void Op_rel() {
    if (isOp_rel(t)) {
        t = Analex(fd);
    } else {
        erro("Expected a relational operator (==, !=, <, etc.).");
    }
}

bool isOp_rel(TOKEN token) {
    return (t.cat == SN &&
        (t.code == EQUALITY   || t.code == DIFERENTE ||
         t.code == LESS_THAN    || t.code == GREATER_THAN   ||
         t.code == LESS_THAN_OR_EQUAL  || t.code == GREATER_EQUAL));
}