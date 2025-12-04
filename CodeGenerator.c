#include "CodeGenerator.h"

static FILE* output;

void InitializeGenerator(const char* file_name) {
    output = fopen(file_name, "w");

    if (output == NULL) 
    {
        printf("Could not open output file '%s'.\n", file_name);
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
