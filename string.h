/*
 ** 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			string.h
 *	Description		Header file for strings
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef STRINGH 
#define STRINGH

#define STR_LEN_INC 8 // Size of the first alloc & next reallocs of the string

typedef struct {
	char* data;
	int size;
	int capacity;
} String;

int stringInit(String** s);
void stringFree(String *s);
int stringClear(String *s);
int stringCpy(String *s, char* c);
int stringAddData(String *s1, char c);
void makeStringLowerCase(String *s);
int stringCmpConstString(String *s1, char *s2);
char *stringGetString(String *s);
int stringGetSize(String *s);
void stringToChar(char **c, String *s);
void concatToString(String *s, char *c);

String *intToString(int n);
String *doubleToString(double n);
int stringToInt(String *s);
double stringToDouble(String *s);

#endif