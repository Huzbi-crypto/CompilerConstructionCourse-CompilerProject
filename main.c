#include <stdio.h>
#include <stdlib.h>
#include "Anasynt.h"
#include "CodeGenerator.h" 

IDENTIFIER_TABLE tableIdentifiers; 

int main(int argc, char *argv[]) {
    // Check if the file name was passed as an argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_file.cshort>\n", argv[0]);
        return 1;
    }

    // Open the source code file for reading
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        fprintf(stderr, "ERROR: Could not open the file '%s'\n", argv[1]);
        return 1;
    }

    // Initializes the code generator.
    InitializeGenerator("result.mp");

    // Call the initial function of the Syntactic Analyzer
    printf("Starting syntactic analysis of the file: %s\n\n", argv[1]);

    Prog(); // Parser entry point

    FinalizeGenerator();

    // Updates the success message
    printf("\nAnalysis complete! Code for the Stack Machine saved in 'result.mp'.\n");


    // Close the file and finish
    fclose(fd);
    return 0;
}