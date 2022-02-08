/*
 *  Project			Implementace překladače imperativního jazyka IFJ17.
 *	File			error.c
 *	Description		Error handle, free memory and exit program
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef ERRORC
#define ERRORC

#include "includes.h"

SPointer *pClearList = NULL;

/* Safety exit the program */
void error(int e) {
	pointerClear(); // Free all memory
	exit(e);
}

/* Controls malloc functions */
void *saveMalloc(size_t size) {
	void *tmp = malloc(size);

	// Add pointer to list
	if(pointerAdd(tmp)) return NULL;

	// Return pointer
	return tmp;
}

/* Realloc for string functions */
void *saveRealloc(void *s, size_t size) {

	// Realloc made some issues, I decided to make it with malloc
	char *temp = saveMalloc(size);

	// Save pointer to list
	if(pointerAdd(temp)) return NULL;

	// Copy content
	strcpy(temp, s);

	// Free memory of previous pointer
	saveFree(s);

	// Returns pointer
	return temp;
}

/* Add new pointer to the list */
int pointerAdd(void *s) {

	// We have no pointer, it's done
	if(s == NULL) return FINE;

	// First item in the list
	if(pClearList == NULL) {

		// Malloc first item
		pClearList = malloc(sizeof(SPointer));
		if(pClearList == NULL) return INTERN_ERR;

		// Setting up the pointer
		pClearList->p = s;
		pClearList->next = NULL;
		return FINE;
	}

	// Inicialization
	SPointer *p = pClearList;
	SPointer *prev = NULL;

	// We have to check, if the pointer is already there
	for(; p != NULL; p = p->next) {
		if(p->p == s) return FINE; // We don't need to save it again
		prev = p;
	}

	// Malloc new item and connect it to the list
	prev->next = malloc(sizeof(SPointer));
	if(prev->next == NULL) return INTERN_ERR;
	prev->next->p = s;
	prev->next->next = NULL;

	return FINE;
}

/* Controlled for free the memory */
void saveFree(void *s) {
	if(pClearList == NULL) return; // No content, is not there

	// If first item in the list
	SPointer *p = pClearList;
	if(p->p == s) {

		// We have new first
		pClearList = p->next;

		// Free memory
		free(p->p);
		free(p);
		return;
	}

	// We have to find the pointer
	SPointer *prev = NULL;
	for(; p != NULL; p = p->next) {

		// We have the pointer
		if(p->p == s) {
			// Connect the list
			prev->next = p->next;

			// Free the memory
			free(p->p);
			free(p);
			break;
		}
		prev = p;
	}
}

/* Clear all the memory */
void pointerClear() {
	if(pClearList == NULL) return; // Nothing to clear

	// Step by step
	SPointer *tmp;
	for(SPointer *p = pClearList; p != NULL; p = tmp) {

		// Free the item and it's pointer
		tmp = p->next;
		free(p->p);
		free(p);
	}

	// Reset
	pClearList = NULL;
}

#endif