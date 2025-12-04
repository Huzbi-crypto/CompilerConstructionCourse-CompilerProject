#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include <stdarg.h> 
#include <stdlib.h> 


/**
 * @brief Function that opens the output file.
 * @param file_name Output file name
 */
void InitializeGenerator(const char* file_name);


/**
 * @brief Function that closes the output file.
 */
void FinalizeGenerator();



/**
 * @brief Main code generation function.
 * @param f Next instruction for code generation.
 */
void GenerateCode(const char* f, ...);


#endif // CODE_GENERATOR_H