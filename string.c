/*
 * 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			string.c
 *	Description		Operations with unlimited long strings
 *	Author			Michal Zilvar (xzilva02)
 */

#include "includes.h"

/* Init string for the first use */
int stringInit(String** s) {

	// Already initialized, free it
	if (*s != NULL) stringFree(*s);

	// Alloc the memory
	*s = saveMalloc(sizeof(String));
	if(*s == NULL) return INTERN_ERR;

	// Initialize it
	(*s)->data = NULL;
	return stringClear(*s);
}

/* Free the string with it's data */
void stringFree(String *s) {
	if(s == NULL) return;

	// Data allocation, free all
	if(s->data != NULL) saveFree(s->data);
	saveFree(s);
}

/* Removes data and sets defaults */
int stringClear(String *s) {

	// We have to init it first
	if(s == NULL) if(stringInit(&s)) return INTERN_ERR;

	// If data free them
	if(s->data != NULL) saveFree(s->data);

	// Malloc memory for the data
	s->data = saveMalloc(STR_LEN_INC * sizeof(char));
	if(s->data == NULL) return INTERN_ERR;

	// Set defaults
	s->data[0] = '\0';
	s->size = 0;
	s->capacity = STR_LEN_INC;

	return FINE;
}

/* Add 1 char to a string*/
int stringAddData(String *s, char c) {

	// If is not, initialize string
	if(s == NULL) if(stringInit(&s)) return INTERN_ERR;

	// String is full, we need more memory
	if((s->size +1) >= s->capacity) {

		// Realloc it with help function
		s->data = saveRealloc(s->data, (s->size +STR_LEN_INC) * sizeof(char));
		if (s->data == NULL) return INTERN_ERR;

		// Capacity raised
		s->capacity = s->size + STR_LEN_INC;
	}

	// Add char, inc. len and add \0
	s->data[s->size] = c;
	s->size = s->size + 1;
	s->data[s->size] = '\0';
	return FINE;
}

/* Copy char to our String */
int stringCpy(String *s, char* c) {

	// Completely clear String or Init
	if(stringClear(s)) return INTERN_ERR;

	// Copy char by char
	for(int i = 0; c[i] != '\0'; i++) {
		if(stringAddData(s, c[i]))
			return INTERN_ERR;
	}

	return FINE;
}

/* Make String lower case */
void makeStringLowerCase(String *s) {
	if(s == NULL) return;

	// Change chars
	for(int i = 0; i < s->size; i++)
		(s->data)[i] = tolower((s->data)[i]);
}

/* Compares String with char* */
int stringCmpConstString(String *s1, char *s2) {

	// Defined returns
	if(s1 == NULL && s2 == NULL) return FINE;
	if(s1 == NULL) return INTERN_ERR;

	// Returns
	return strcmp(s1->data, s2);
}

/* Sometimes we need the concrete string */
char *stringGetString(String *s) {
	if(s == NULL) return NULL;
	
	return s->data;
}

/* Like strlen */
int stringGetSize(String *s) {
	if (s != NULL) return 0;

	return s->size;
}

/* Converts string to char */
void stringToChar(char **c, String *s) {
	if(s == NULL) return;

	// Size 0 means only \0
	if (s->size == 0) {
		(*c) = saveMalloc(sizeof(char));
		(*c)[0] = '\0';
	}
	else{
		(*c) = saveMalloc((sizeof(char)) * (s->size + 1));
		strncpy(*c, s->data, s->size);
		(*c)[s->size] = '\0';
	}
}

/* Add char* to string */
void concatToString(String *s, char *c) {
	for(int i = 0; c[i] != '\0'; i++)
		stringAddData(s, c[i]);
}

/* Transform integer to String */
String *intToString(int n) {

	// 10 chars for 32 bit, enough k
	char temp[sizeof(int)*4];
	sprintf(temp, "%d", n);

	// Create String and save the int
	String *s;
	if(stringInit(&s)) return NULL;

	concatToString(s, temp);
	return s;
}

/* Transform double to String */
String *doubleToString(double n) {

	// 10 chars for 32 bit, enough k
	char temp[sizeof(double)*4];
	sprintf(temp, "%g", n);

	// Create String and save the int
	String *s = NULL;

	if(stringInit(&s)) return NULL;

	concatToString(s, temp);
	return s;
}

/* Transform String to int */
int stringToInt(String *s) {

	int n;
	sscanf(stringGetString(s), "%d", &n);
	return n;
}

/* Transform String to double */
double stringToDouble(String *s) {

	double n;
	sscanf(stringGetString(s), "%lf", &n);
	return n;
}