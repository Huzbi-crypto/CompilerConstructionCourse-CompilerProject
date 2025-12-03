#include "CodeGenerator.h"

static FILE* output;

void InitializeGenerator(const char* nome_arquivo) {
    output = fopen(nome_arquivo, "w");

    if (output == NULL) 
    {
        printf("Could not open output file '%s'.\n", nome_arquivo);
        exit(1);
    }
}

void FinalizeGenerator() {
    if (output != NULL) 
    {
        fclose(output);
        output = NULL; 
    }
}

void GenerateCode(const char* f, ...) {
    if (output == NULL) 
    {
        return;
    }

    va_list args;
    va_start(args, f);
    vfprintf(output, f, args);
    fprintf(output, "\n");
    va_end(args);
}
