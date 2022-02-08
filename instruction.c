/*
 * 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			instruction.c
 *	Description		Handles output, instructions and output file
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef INSTRUCTIONC
#define INSTRUCTIONC

#include "includes.h"

/* Open output file where will be the instructions */
void openOutput() {

	/* Every instruction list starts with this TAG */
	fprintf(stdout, ".IFJcode17\n");
}

/* Universal function to add an instruction */
void instruction(char *s, String *a1, String *a2, String *a3, char *f1, char *f2, char *f3) {
	
	// There is a possibility of infinite String in there
	String *format = NULL;
	stringInit(&format);
	stringClear(format);

	// Add instruction type
	concatToString(format, s);

	// Add first argument
	if(a1 != NULL) {
		stringAddData(format, ' ');

		if(f1 != NULL) {
			concatToString(format, f1);
			stringAddData(format, '@');
		}
		concatToString(format, a1->data);
	}

	// Add second argument
	if(a2 != NULL) {
		stringAddData(format, ' ');

		if(f2 != NULL) {
			concatToString(format, f2);
			stringAddData(format, '@');
		}
		concatToString(format, a2->data);
	}

	// Add thirth argument
	if(a3 != NULL) {
		stringAddData(format, ' ');

		if(f3 != NULL) {
			concatToString(format, f3);
			stringAddData(format, '@');
		}
		concatToString(format, a3->data);
	}

	// Write the formated output to the file and LF
	fprintf(stdout, "%s\n", stringGetString(format));

	// Free some temporary memory
	stringFree(format);
}

//
// Writes all the necessary instructions for the predefined functions( substr, length, asc, chr ) to stdout
//
void writePredefined(){
	fprintf(stdout, "LABEL substr\n");
	fprintf(stdout, "DEFVAR LF@s\n");
	fprintf(stdout, "DEFVAR LF@i\n");
	fprintf(stdout, "DEFVAR LF@n\n");
	fprintf(stdout, "POPS LF@n\n");
	fprintf(stdout, "POPS LF@i\n");
	fprintf(stdout, "POPS LF@s\n");
	fprintf(stdout, "DEFVAR LF@temp1\n");
	fprintf(stdout, "DEFVAR LF@temp2\n");
	fprintf(stdout, "EQ LF@temp1 LF@i int@0\n");
	fprintf(stdout, "PUSHS LF@temp1\n");
	fprintf(stdout, "LT LF@temp1 LF@i int@0\n");
	fprintf(stdout, "PUSHS LF@temp1\n");
	fprintf(stdout, "ORS\n");
	fprintf(stdout, "STRLEN LF@temp1 LF@s\n");
	fprintf(stdout, "EQ LF@temp2 int@0 LF@temp1\n");
	fprintf(stdout, "PUSHS LF@temp2\n");
	fprintf(stdout, "ORS\n");
	fprintf(stdout, "GT LF@temp2 LF@i LF@temp1\n");
	fprintf(stdout, "PUSHS LF@temp2\n");
	fprintf(stdout, "ORS\n");
	fprintf(stdout, "PUSHS bool@true\n");
	fprintf(stdout, "JUMPIFNEQS IF1\n");
	fprintf(stdout, "PUSHS string@\n");
	fprintf(stdout, "RETURN\n");
	fprintf(stdout, "LABEL IF1\n");
	fprintf(stdout, "STRLEN LF@temp1 LF@s\n");
	fprintf(stdout, "PUSHS LF@n\n");
	fprintf(stdout, "PUSHS LF@temp1\n");
	fprintf(stdout, "PUSHS LF@i\n");
	fprintf(stdout, "SUBS\n");
	fprintf(stdout, "GTS\n");
	fprintf(stdout, "LT LF@temp1 LF@n int@0\n");
	fprintf(stdout, "PUSHS LF@temp1\n");
	fprintf(stdout, "ORS\n");
	fprintf(stdout, "PUSHS bool@true\n");
	fprintf(stdout, "SUB LF@i LF@i int@1\n");
	fprintf(stdout, "JUMPIFNEQS IF2\n");
	fprintf(stdout, "STRLEN LF@temp1 LF@s\n");
	fprintf(stdout, "SUB LF@n LF@temp1 LF@i\n");
	fprintf(stdout, "LABEL IF2\n");
	fprintf(stdout, "MOVE LF@temp1 string@\n");
	fprintf(stdout, "LABEL LOO\n");
	fprintf(stdout, "GETCHAR LF@temp2 LF@s LF@i\n");
	fprintf(stdout, "CONCAT LF@temp1 LF@temp1 LF@temp2\n");
	fprintf(stdout, "ADD LF@i LF@i int@1\n");
	fprintf(stdout, "SUB LF@n LF@n int@1\n");
	fprintf(stdout, "PUSHS LF@n\n");
	fprintf(stdout, "PUSHS int@0\n");
	fprintf(stdout, "JUMPIFNEQS LOO\n");
	fprintf(stdout, "PUSHS LF@temp1\n");
	fprintf(stdout, "RETURN\n");
	fprintf(stdout, "LABEL length\n");
	fprintf(stdout, "DEFVAR LF@s\n");
	fprintf(stdout, "POPS LF@s\n");
	fprintf(stdout, "DEFVAR LF@temp\n");
	fprintf(stdout, "STRLEN LF@temp LF@s\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "RETURN\n");
	fprintf(stdout, "LABEL asc\n");
	fprintf(stdout, "DEFVAR LF@s\n");
	fprintf(stdout, "DEFVAR LF@i\n");
	fprintf(stdout, "POPS LF@i\n");
	fprintf(stdout, "POPS LF@s\n");
	fprintf(stdout, "DEFVAR LF@temp\n");
	fprintf(stdout, "STRLEN LF@temp LF@s\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "PUSHS LF@i\n");
	fprintf(stdout, "LTS\n");
	fprintf(stdout, "LT LF@temp LF@i int@1\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "ORS\n");
	fprintf(stdout, "PUSHS bool@true\n");
	fprintf(stdout, "JUMPIFNEQS IF3\n");
	fprintf(stdout, "PUSHS int@0\n");
	fprintf(stdout, "RETURN\n");
	fprintf(stdout, "LABEL IF3\n");
	fprintf(stdout, "ADD LF@i LF@i int@1\n");
	fprintf(stdout, "STRI2INT LF@temp LF@s LF@i\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "RETURN\n");
	fprintf(stdout, "LABEL chr\n");
	fprintf(stdout, "DEFVAR LF@i\n");
	fprintf(stdout, "POPS LF@i\n");
	fprintf(stdout, "DEFVAR LF@temp\n");
	fprintf(stdout, "INT2CHAR LF@temp LF@i\n");
	fprintf(stdout, "PUSHS LF@temp\n");
	fprintf(stdout, "RETURN\n");
}

#endif