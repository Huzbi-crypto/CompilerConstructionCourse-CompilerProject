# Cshort Compiler

## About The Project

This repository contains a compiler for **TinyC**, a simple C-like language, developed for the Compiler Construction course. 
The compiler performs lexical, syntax, and semantic analysis, followed by code generation, converting `.cshort` files into an `result.mp` output file.

## How to Compile the Code

```script
cd src/
gcc -o compiler Analex.c main.c Anasint.c Anasem.c CodeGenerator.c TabIdent.c
```

## How to Run the Compiler

```script
./compiler test.cshort
```

## Output
The compiled output will be generated as `result.mp`

