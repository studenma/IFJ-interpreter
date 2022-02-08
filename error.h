/*
 * 	Project			Implementace překladače imperativního jazyka IFJ17.
 *	File			error.h
 *	Description		Header file for error.c
 *	Author			Michal Zilvar (xzilva02)
 *	Last update		12:16, 29-10-2017
 */

#ifndef ERRORH
#define ERRORH

//
// List of allocated pointers
//
typedef struct pointerList {
	void *p;					// Pointer
	struct pointerList *next;	// Pointer to next pointer
} SPointer;

void error(int e);

void *saveMalloc(size_t size);
void saveFree(void *s);
void *saveRealloc(void *s, size_t size);
int pointerAdd(void *s);	// Add new pointer
void pointerClear();		// Free all memory

#endif