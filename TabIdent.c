#include <stdio.h>
#include <stdlib.h>
#include "TabIdent.h"

void StartTableID() {
    tableIdentifiers.tamTabela = 0;  
}

/*
* @brief Returns the index of the Identifier in the Identifier Table
* @param nameId Identifier name
*/
int SearchTableID(const char *nomeId) {

    for (int i = 0; i < tableIdentifiers.tamTabela; i++) {
        if (!(strcmp(nomeId, tableIdentifiers.identifiers[i].name))) {
            return i;
        }
    }
    return -1;
}

int SearchTableIDSameScope(const char *nomeId, SCOPE scope) {

    for (int i = 0; i < tableIdentifiers.tamTabela; i++) {
        if (!(strcmp(nomeId, tableIdentifiers.identifiers[i].name))) {
            if( tableIdentifiers.identifiers[i].scope == scope) {
                return i;
            }
        }
    }

    return -1;
}

int InsertTableID(const char *nomeId, CATEGORY_IDENTIFIER catId, SCOPE scope, DATA_TYPE type, bool isZombie, bool isArray, int tamArray, int address) {
    if (tableIdentifiers.tamTabela == MAX_IDENTIF) {
        printf("Overflow in the identifier table!");
    }

    int i = tableIdentifiers.tamTabela;
    strcpy(tableIdentifiers.identifiers[i].name, nomeId);

    // Use the address passed as a parameter instead of 'i'
    tableIdentifiers.identifiers[i].address = address;
    
    tableIdentifiers.identifiers[i].category = catId;
    tableIdentifiers.identifiers[i].scope = scope;
    tableIdentifiers.identifiers[i].type = type;
    tableIdentifiers.identifiers[i].zombie = isZombie;
    tableIdentifiers.identifiers[i].array = isArray;
    tableIdentifiers.identifiers[i].tamArray = tamArray;

    tableIdentifiers.tamTabela++;
    return i;
}

int removeLastIDInserted(int posicaoUltimoInserido)
{
    tableIdentifiers.identifiers[posicaoUltimoInserido].name[0] = '\0';
    tableIdentifiers.identifiers[posicaoUltimoInserido].category = CAT_EMPTY;
    tableIdentifiers.identifiers[posicaoUltimoInserido].type = TYPE_VOID;
    tableIdentifiers.identifiers[posicaoUltimoInserido].scope = GLOBAL;
    tableIdentifiers.identifiers[posicaoUltimoInserido].address = 0;
    tableIdentifiers.identifiers[posicaoUltimoInserido].array = false;
    tableIdentifiers.identifiers[posicaoUltimoInserido].tamArray = 0;
    tableIdentifiers.identifiers[posicaoUltimoInserido].zombie = false;

    return posicaoUltimoInserido - 1;
}
