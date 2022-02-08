/*
 *  Project         Implementace překladače imperativního jazyka IFJ17.
 *	File			macros.h
 *	Description		Help for debugging, etc.
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef MACROS
#define MACROS

/* Přepínače */
//#define DEBUG_ON

/* Definice */
#ifdef DEBUG_ON
#define $(...); puts("DEBUG: " #__VA_ARGS__);
#define $$(...); printf("DEBUG: " __VA_ARGS__);
#else
#define $(...);
#define $$(...);
#endif

#endif