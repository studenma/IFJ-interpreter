// Project: Implementace překladače imperativního jazyka IFJ17.
// Magic constants for errors and lexical tokens
// Author: xzaple36 (Jakub Zapletal)

#ifndef DEFINEH
#define DEFINEH

//
//  ERRORS
//

#define LEX_ERR             1
#define SYN_ERR             2
#define DEF_ERR             3
#define TYPE_ERR            4
#define SEM_ERR             6
#define INTERN_ERR          99

// 
//  TOKENS
//
#define T_ID                100 // IDENTIFIER

//  LITERALS
#define L_INT          	    101
#define L_FLOAT             102
#define L_STRING            103

//  TYPE KEYWORDS
#define T_INTEGER           104
#define T_DOUBLE            105
#define T_STRING            106

//  OPERATORS
#define T_LB                108 // (
#define T_RB                109 // )
#define T_COMMA             110 // ,
#define T_SEMICOLON         111 // ;
#define T_ADD               112 // +
#define T_SUB               113 // -
#define T_TIMES             114 // *
#define T_DIV               115 // /
#define T_GT                116 // >
#define T_LT                117 // <
#define T_GTE               118 // >=
#define T_LTE               119 // <=
#define T_EQ                120 // =
#define T_NEQ               121 // <>
#define T_IDIV			    122 /* \ */

//
//  OTHER KEYWORDS
//
#define T_AND               150
#define T_AS                151
#define T_ASC               152
#define T_BOOLEAN           153
#define T_CHR               154
#define T_CONTINUE          155
#define T_DECLARE           156
#define T_DIM               157
#define T_DO                158
#define T_ELSE              159
#define T_ELSEIF            160
#define T_END               161
#define T_EXIT              161
#define T_FALSE             162
#define T_FOR               163
#define T_FUNCTION          164
#define T_IF                165
#define T_INPUT             166
#define T_LENGTH            167
#define T_LOOP              168
#define T_OR                169
#define T_NEXT              170
#define T_NOT               171
#define T_PRINT             172
#define T_RETURN            173
#define T_SHARED            174
#define T_SCOPE             175
#define T_STATIC            176
#define T_SUBSTR            177
#define T_THEN              178
#define T_TRUE              179
#define T_WHILE             180

#define T_EOL               198 // \n || \r
#define T_EOF               199

#define FINE                0
#define FAIL                1

//
// Magic constants for data types and metaTypes
//
#define INTEGER             1
#define DOUBLE              2
#define STRING              3
#define BOOLEAN             4

#define VARIABLE            1
#define FUNCTION            2

#endif