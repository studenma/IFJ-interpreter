/*
 * 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			scanner.c
 *	Description		Source file for lexical analysis
 *	Author			Michal Zilvar (xzilva02)
 */

#ifndef SCANNERC
#define SCANNERC

#include "includes.h"

int asciiVal = 0;
int asciiCount = 0;

//
// Read, validates and returns a token from input
//
int getToken(String *s, int *cursor){
	char c;
	int shunt = LEX_WAITING;
	stringClear(s);
	(*cursor) = 0;
	do {
		c = getc(stdin);
		(*cursor)++;

		if(c == EOF && (shunt == LEX_BLOCK || shunt == LEX_BLOCKE)) return LEX_ERR;
		if(c == EOF && shunt != LEX_WAITING && shunt != LEX_KEYWORD) return T_EOF;

		switch(shunt) {
			/* Normal reading */
			case LEX_WAITING:

				/* EOL */
				if(c == '\n') return T_EOL;
				else if(c == '\r') return T_EOL;

				/* Blank before a token, continue reading */
				else if(isspace(c)) shunt = LEX_WAITING;

				/* Complicated symbols */
				else if(c == '!') shunt = LEX_NOT;
				else if(c == '>') shunt = LEX_GREATER;
				else if(c == '<') shunt = LEX_SMALLER;
				else if(c == '\'') shunt = LEX_COMMENT;

				/* Basic alone symbols */
				else if(c == '(') return T_LB;
				else if(c == ')') return T_RB;
				else if(c == ',') return T_COMMA;
				else if(c == ';') return T_SEMICOLON;

				/* Operations */
				else if(c == '+') return T_ADD;
				else if(c == '-') return T_SUB;
				else if(c == '*') return T_TIMES;
				else if(c == '\\') return T_IDIV;
				else if(c == '/') shunt = LEX_BLOCKDIV;

				else if(c == '=') return T_EQ;
				else if(c == EOF) return T_EOF;

				/* Otherwise */
				else {
					/* Numbers and keywords (save the token) */
					if(isdigit(c)) shunt = LEX_DIGIT;
					else if(isalpha(c) || c == '_') shunt = LEX_KEYWORD;
					else return LEX_ERR;

					stringAddData(s, c);
				}
				break;

			/* Is it comment or div */
			case LEX_BLOCKDIV:
				if(c == '\'') shunt = LEX_BLOCK;
				else {
					ungetc(c, stdin);
					(*cursor)--;
					return T_DIV;
				}
				break;

			/* We are in a comment */
			case LEX_BLOCK:
				if(c == '\'') shunt = LEX_BLOCKE;
				break;

			/* End of block comment */
			case LEX_BLOCKE:
				if(c == '/') shunt = LEX_WAITING;
				else shunt = LEX_BLOCK;
				break;

			/* String like !"Hello world"	*/
			case LEX_NOT:
				if(c == '"') shunt = LEX_STRING;
				else return LEX_ERR;
				break;

			/* We have a string! */
			case LEX_STRING:
				/* End of string? */
				if(c == '"') return L_STRING;

				/* If escape sequence or add char */
				if(c == '\\') shunt = LEX_STRINGE;
				else if(c <= 32) {
					char buf[4];
					if(c < 10) sprintf(buf, "\\00%d", c);
					else sprintf(buf, "\\0%d", c);
					concatToString(s, buf);
				}
				else stringAddData(s, c);
				break;

			case LEX_STRINGE:
				if(isdigit(c)) {
					/* ASCII value of a char */
					asciiVal = 10*asciiVal + (c-'0');
					asciiCount++;

					/* We have its value */
					if(asciiCount == 3) {
						/* Is valid? */
						if(asciiVal < 1 || asciiVal > 255) return LEX_ERR;

						stringAddData(s, asciiVal);
						asciiCount = 0;
						asciiVal = 0;

						shunt = LEX_STRING;
					}
				}
				else
				{
					/* Looked as an ascii value, but was not */
					if(asciiVal) return LEX_ERR;

					shunt = LEX_STRING;
					/* Checking valid */
					switch(c) {
						case '"': concatToString(s, "\\034"); break;
						case 'n': concatToString(s, "\\012"); break;
						case 't': concatToString(s, "\\011"); break;
						case '\\':concatToString(s, "\\092"); break;
						default: return LEX_ERR;
					}
				}
				break;

			/* > or >= */
			case LEX_GREATER:
				if(c == '=') return T_GTE;
				ungetc(c, stdin);
				(*cursor)--;
				return T_GT;

			/* < or <= or <> */
			case LEX_SMALLER:
				if(c == '=') return T_LTE;
				else if(c == '>') return T_NEQ;
				ungetc(c, stdin);
				(*cursor)--;
				return T_LT;

			/* Comment like 'comm */
			case LEX_COMMENT:
				if(c == '\n') return T_EOL;
				else if(c == '\r') return T_EOL;
				break;

			/* 123 or 12.3 */
			case LEX_DIGIT:
				if(isdigit(c)) {}
				else if(c == '.') shunt = LEX_FLOATF;
				else if(c == 'e' || c == 'E') shunt = LEX_EFLOATC;
				else {
					ungetc(c, stdin);
					(*cursor)--;
					return L_INT;
				}
				stringAddData(s, c);
				break;

			/* Float at least 1 digit */
			case LEX_FLOATF:
				if(isdigit(c)) {}
				else return LEX_ERR;
				stringAddData(s, c);
				shunt = LEX_FLOAT;
				break;

			/* 12.3 */
			case LEX_FLOAT:
				if(isdigit(c)) {}
				else if(c == 'e' || c == 'E') shunt = LEX_EFLOATC;
				else {
					ungetc(c, stdin);
					(*cursor)--;
					return L_FLOAT;
				}
				stringAddData(s, c);
				break;

			/* 12e+10 */
			case LEX_EFLOATC:
				if(c == '+' || c == '-') {
					shunt = LEX_EFLOATF;
					stringAddData(s, c);
				}
				else if(isdigit(c)) {
					shunt = LEX_EFLOAT;
					stringAddData(s, c);
				}
				else return LEX_ERR;
				break;

			/* 12e+1 */
			case LEX_EFLOATF:
				if(!isdigit(c)) return LEX_ERR;
				stringAddData(s, c);
				shunt = LEX_EFLOAT;
				break;

			/* 12e+10 */
			case LEX_EFLOAT:
				if(isdigit(c)) {}
				else {
					ungetc(c, stdin);
					(*cursor)--;
					return L_FLOAT;
				}
				stringAddData(s, c);
				break;

			/* ID or keyword */
			case LEX_KEYWORD:

				if(isalnum(c) || c == '_') stringAddData(s, c);
				else {
					ungetc(c, stdin);
					(*cursor)--;

					/* Lower case keyword */
					makeStringLowerCase(s);

					/* Valtypes */
					if(!stringCmpConstString(s, "integer"))	return T_INTEGER;
					if(!stringCmpConstString(s, "double"))	return T_DOUBLE;
					if(!stringCmpConstString(s, "string"))	return T_STRING;

					/* Other keywords */
					if(!stringCmpConstString(s, "as"))		return T_AS;
					//if(!stringCmpConstString(s, "asc"))		return T_ASC;
					if(!stringCmpConstString(s, "declare"))	return T_DECLARE;
					if(!stringCmpConstString(s, "dim"))		return T_DIM;
					if(!stringCmpConstString(s, "do"))		return T_DO;
					if(!stringCmpConstString(s, "else"))	return T_ELSE;
					if(!stringCmpConstString(s, "end"))		return T_END;
					//if(!stringCmpConstString(s, "chr"))		return T_CHR;
					if(!stringCmpConstString(s, "function"))return T_FUNCTION;
					if(!stringCmpConstString(s, "if"))		return T_IF;
					if(!stringCmpConstString(s, "input"))	return T_INPUT;
					//if(!stringCmpConstString(s, "length"))	return T_LENGTH;
					if(!stringCmpConstString(s, "loop"))	return T_LOOP;
					if(!stringCmpConstString(s, "print"))	return T_PRINT;
					if(!stringCmpConstString(s, "return"))	return T_RETURN;
					if(!stringCmpConstString(s, "scope"))	return T_SCOPE;
					//if(!stringCmpConstString(s, "subStr"))	return T_SUBSTR;
					if(!stringCmpConstString(s, "then"))	return T_THEN;
					if(!stringCmpConstString(s, "while"))	return T_WHILE;
					if(!stringCmpConstString(s, "and"))		return T_AND;
					if(!stringCmpConstString(s, "boolean"))	return T_BOOLEAN;
					if(!stringCmpConstString(s, "continue"))return T_CONTINUE;
					if(!stringCmpConstString(s, "elseif"))	return T_ELSEIF;
					if(!stringCmpConstString(s, "exit"))	return T_EXIT;
					if(!stringCmpConstString(s, "false"))	return T_FALSE;
					if(!stringCmpConstString(s, "for"))		return T_FOR;
					if(!stringCmpConstString(s, "next"))	return T_NEXT;
					if(!stringCmpConstString(s, "not"))		return T_NOT;
					if(!stringCmpConstString(s, "or"))		return T_OR;
					if(!stringCmpConstString(s, "shared"))	return T_SHARED;
					if(!stringCmpConstString(s, "static"))	return T_STATIC;
					if(!stringCmpConstString(s, "true"))	return T_TRUE;
																											$$("identifier %s\n", stringGetString(s));
					return T_ID;
				}
				break;
		}
	} while ((2+2*2+2*2) == (3*3+3/3));
}

//
// Returns a token to input
//
void pushbackAttr(int l) {
	fseek(stdin, -l, SEEK_CUR);																				$$("pushbackAttr(%d);\n", l);
}

#endif
