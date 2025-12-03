#ifndef ANASINT_H
#define ANASINT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "Analex.h"
#include "TabIdent.h"
#include "Anasem.h"
#include "CodeGenerator.h"

typedef enum { NO_DECL, DECL_VAR, DECL_PROT, DECL_PROT_UNICO } DECL_FLAG;

/* Global Variables */
extern TOKEN t;
extern TOKEN tLookahead;
extern FILE *fd;
extern int lineCont;
extern SCOPE scope;

/* Functions of Grammar Production Rules*/
void Prog();
DECL_FLAG Decl();
int body_func();
void Decl_var();
void Type();
void Type_param();
void Cmd();
void cmd_cont(TOKEN target_id);
void Atrib();
void factor_cont(IDENTIFIER func_id);

// Functions that now return the type of the analyzed expression
DATA_TYPE Expr(); 
DATA_TYPE Expr_simp();
DATA_TYPE Termo();
DATA_TYPE Fator();

void Op_rel();

/* Utility functions */

void process_array();
bool isOp_rel(TOKEN token);

extern IDENTIFIER_TABLE tableIdentifiers;

#endif // ANASINT_H