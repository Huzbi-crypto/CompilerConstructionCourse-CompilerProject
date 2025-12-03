#ifndef _TAB_IDENT_
#define _TAB_IDENT_

#include <stdbool.h>
#include <string.h>
#include "Analex.h"

#define MAX_IDENTIF 100

typedef enum {
    CAT_EMPTY,
    CAT_VAR,
    CAT_FUNC,
    CAT_PARAM
} CATEGORY_IDENTIFIER;

typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_BOOL,
    UNKNOWN_TYPE
} DATA_TYPE;

typedef enum {
    GLOBAL,
    LOCAL
} SCOPE;

typedef struct {
    char name[MAX_LEXEME_SIZE];
    CATEGORY_IDENTIFIER categoria; 
    DATA_TYPE type; // Symbol data type
    SCOPE scope; // Symbol scope (global, local)
    int address;
    int tamArray;
    bool array;
    bool zombie; // If it is outside the scope
} IDENTIFIER;

typedef struct {
    int tamTabela;
    IDENTIFIER identifiers[MAX_IDENTIF];
} IDENTIFIER_TABLE;

/* Global variables*/
extern TOKEN t;
extern TOKEN lookahead;
extern IDENTIFIER_TABLE tableIdentifiers;

/* Signature of Functions */
void StartTableID();
int SearchTableID(const char *nameId);
int SearchTableIDSameScope(const char *nameId, SCOPE scope);
int InsertTableID(const char *nameId, CATEGORY_IDENTIFIER catId, SCOPE scope, DATA_TYPE type, bool isZombie, bool isArray, int tamArray, int address);
int removeLastIDInserted(int lastAddedPosition);

#endif