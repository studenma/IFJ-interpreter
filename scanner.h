/*
 * 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			scanner.h
 *	Description		Header file for scanner.c
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef SCANNERH
#define SCANNERH

#define LEX_WAITING		0
#define LEX_DIGIT		1
#define LEX_FLOAT		2
#define LEX_KEYWORD		3
#define LEX_NOT			4
#define LEX_EQUAL		5
#define LEX_GREATER		6
#define LEX_SMALLER		7
#define LEX_STRING		8
#define LEX_STRINGE		9
#define LEX_EFLOATC		10
#define LEX_EFLOATF		11
#define LEX_EFLOAT		12
#define LEX_FLOATF		13
#define LEX_COMMENT		14
#define LEX_BLOCKDIV	15
#define LEX_BLOCK		16
#define LEX_BLOCKE		17

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "define.h"
#include "string.h"
#include "macros.h"

int getToken(String *s, int *cursor);	// Returns error/token type and saves the value
void pushbackAttr(int l);				// We need to push back

#endif