/*
 *  Project     Implementace překladače imperativního jazyka IFJ17.
 *	File			instruction.h
 *	Description		Header file for instruction.c
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef INSTRUCTIONH
#define INSTRUCTIONH

void instruction(char *s, String *a1, String *a2, String *a3, char *f1, char *f2, char *f3); // Add instruction
void openOutput(); // Set file to output
void writePredefined(); // Write IFJcode17 instructions for predefined functions

#endif