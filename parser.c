/*
 *  Project			Implementace překladače imperativního jazyka IFJ17.
 *	File			parser.c
 *	Description		Source file for syntax analysis
 *	Author			Radek Wildmann (xwildm00), Jakub Zapletal (xzaple36)
 */
#include "includes.h"

int token = 0; // token as defined in define.h
int tokenSize; // length of the attribute portion of token
int ifCounter = 0; // depth counter for nested ifs
int loopCounter = 0; // depth counter for nested while loops

//
// helper strings for analysis or instruction generation
//
String *attribute = NULL;
String *variableName = NULL;
String *functionName = NULL;
String *inFunction = NULL;
String *operand1 = NULL;
String *operand2 = NULL;
String *operand3 = NULL;

int type = 0; // 1 => int; 2 => double; 3 => string; data type of current variable or function
int expressionType = 0; // 1 => int; 2 => double; 3 => string; data type of result of precedence analysis
tokenparam precedenceBuffer[100];

nodePtr *currentSymtable; // lokalni tabulka symbolu 
extern nodePtr symtable;

//precedence table
//COL	- INPUT CHAR
//ROW	- TOP TERMINAL ON STACK
int precTable[15][15] = {
	//		 ID		(	)	+	-	*	/	\	<	>  <=  >=  ==  <>	$
	/* ID */{ E,	E,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R },
	/*  ( */{ L,	L,	EQ,	L,	L,	L,	L,	L,	L,	L,	L,	L,	L,	L,	E },
	/*  ) */{ E,	E,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R },
	/*  + */{ L,	L,	R,	R,	R,	L,	L,	L,	R,	R,	R,	R,	R,	R,	R },
	/*  - */{ L,	L,	R,	R,	R,	L,	L,	L,	R,	R,	R,	R,	R,	R,	R },
	/*  * */{ L,	L,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R },
	/*  / */{ L,	L,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R },
	/*  \ */{ L,	L,	R,	R,	R,	L,	L,	R,	R,	R,	R,	R,	R,	R,	R },
	/*  < */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/*  > */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/* <= */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/* >= */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/*  = */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/* <> */{ L,	L,	R,	L,	L,	L,	L,	L,	E,	E,	E,	E,	E,	E,	R },
	/*  $ */{ L,	L,	E,	L,	L,	L,	L,	L,	L,	L,	L,	L,	L,	L,	E }
};

//
// generates appropriate instructions based on the analysed expression
//
void evalExpr(void *lValue, void *mValue, void *rValue){
	struct value result;
	result.i = 0;
	result.d = 0.0;
	result.s = NULL;
	int firstType;
	int secondType;
	bool intOp = true;
	stringInit(&(result.s));

	if (((tokenparam *)lValue)->token == PRLB && ((tokenparam *)rValue)->token == PRRB){
		expressionType = ((tokenparam *)mValue)->type;
	}
	else if (mValue == NULL){
		if (((tokenparam *)lValue)->identifier){
stringCpy(operand1, ((tokenparam *)lValue)->data->data);
instruction("PUSHS", operand1, NULL, NULL, "LF", NULL, NULL);			
		}
		else{
			switch(expressionType = ((tokenparam *)lValue)->type){
				case INTEGER:
stringCpy(operand1, ((tokenparam *)lValue)->data->data);
instruction("PUSHS", operand1, NULL, NULL, "int", NULL, NULL);	
				break;
				case DOUBLE:
stringCpy(operand1, ((tokenparam *)lValue)->data->data);
instruction("PUSHS", operand1, NULL, NULL, "float", NULL, NULL);	
				break;
				case STRING:
stringCpy(operand1, ((tokenparam *)lValue)->data->data);
instruction("PUSHS", operand1, NULL, NULL, "string", NULL, NULL);	
				break;
				default:
					error(INTERN_ERR);
			}	
		}
		expressionType = ((tokenparam *)lValue)->type;
	}
	else{
		firstType = ((tokenparam *)lValue)->type;
		secondType = ((tokenparam *)rValue)->type;

		if ((firstType == STRING && secondType != STRING) || (firstType != STRING && secondType == STRING)){
			error(TYPE_ERR);
		}

		if (firstType == DOUBLE || secondType == DOUBLE){
			if (((tokenparam *)mValue)->token == PRIDIV){
				if (firstType == DOUBLE){
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("FLOAT2R2EINTS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
					firstType = DOUBLE;
				}
				else if (secondType == DOUBLE){
instruction("FLOAT2R2EINTS", NULL, NULL, NULL, NULL, NULL, NULL);
					secondType = DOUBLE;
				}
			}
			else{
				if (firstType == INTEGER){
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
					firstType = DOUBLE;
				}
				else if (secondType == INTEGER){
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
					secondType = DOUBLE;
				}
				intOp = false;
			}
		}

		switch(((tokenparam *)mValue)->token){
			case PRPLUS:
				if (firstType == STRING && secondType == STRING){
stringCpy(operand2, "$TEMP1");
instruction("DEFVAR", operand2, NULL, NULL, "LF", NULL, NULL);
stringCpy(operand3, "$TEMP2");
instruction("DEFVAR", operand3, NULL, NULL, "LF", NULL, NULL);
instruction("POPS", operand3, NULL, NULL, "LF", NULL, NULL);
instruction("POPS", operand2, NULL, NULL, "LF", NULL, NULL);
instruction("CONCAT", operand2, operand2, operand3, "LF", "LF", "LF");
instruction("PUSHS", operand2, NULL, NULL, "LF", NULL, NULL);
					expressionType = STRING;
				}
				else{
					if (intOp){
						expressionType = INTEGER;
					}
					else{
						expressionType = DOUBLE;
					}
instruction("ADDS", NULL, NULL, NULL, NULL, NULL, NULL);
				}
				break;
			case PRMINUS:
				if (intOp){
					expressionType = INTEGER;
				}
				else{
					expressionType = DOUBLE;
				}
instruction("SUBS", NULL, NULL, NULL, NULL, NULL, NULL);
				break;
			case PRTIMES:
				if (intOp){
					expressionType = INTEGER;
				}
				else{
					expressionType = DOUBLE;
				}
instruction("MULS", NULL, NULL, NULL, NULL, NULL, NULL);
				break;
			case PRDIV:
				if (intOp){
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("DIVS", NULL, NULL, NULL, NULL, NULL, NULL);
				}
				else{
instruction("DIVS", NULL, NULL, NULL, NULL, NULL, NULL);
				}
				expressionType = DOUBLE;
				break;
			case PRIDIV:
				expressionType = INTEGER;
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("DIVS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("FLOAT2R2EINTS", NULL, NULL, NULL, NULL, NULL, NULL);
				break;
			case PRLT:
instruction("LTS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
				break;
			case PRGT:
instruction("GTS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
				break;
			case PRELT:
instruction("LTS", NULL, NULL, NULL, NULL, NULL, NULL);
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("EQS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("ORS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
				break;
			case PREGT:
instruction("GTS", NULL, NULL, NULL, NULL, NULL, NULL);
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("EQS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("ORS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
					break;
			case PREQUAL:
instruction("EQS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
					break;
			case PRNEQ:
instruction("EQS", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("NOTS", NULL, NULL, NULL, NULL, NULL, NULL);
					expressionType = BOOLEAN;
					break;
			default:
				error(SYN_ERR);
				break;
		}
	}
}

//
// Precedence stack functions
//
int stackInit(tStack *stack) { // stack inicialization

	if (!stack)
		error(INTERN_ERR);
	stack->top = -1;
	return FINE;
}

int push(tStack *stack, tokenparam token) { // pushes new terminal to stack
	
	if (stack->top == STACK_SIZE) // if stack is full
		error(INTERN_ERR);
	stack->top++;
	stack->arr[stack->top].token = token.token;
	stack->arr[stack->top].data = token.data;
	stack->arr[stack->top].type = token.type;
	stack->arr[stack->top].identifier = token.identifier;
	return FINE;
}
int pop(tStack *stack, tokenparam *token) { // pops top token
	
	if (stack->top == -1)
		error(INTERN_ERR);
	*token = stack->arr[stack->top];
	stack->top--;
	return FINE;
}

void replaceY(tStack *stack, char a){ // <y za A
	tokenparam buffer[100];
	int i;
	tokenparam NONTtoken;
	NONTtoken.token = NONT;
	for (i = 0; top(stack) != LB; i++){
		pop(stack, &buffer[i]);
	}
	if (i == 1)
		evalExpr(&buffer[0],NULL, NULL);
	else
		evalExpr(&buffer[2],&buffer[1], &buffer[0]);

	NONTtoken.type = expressionType;
	pop(stack, &buffer[i]);
	push(stack, NONTtoken);
}

void changeA(tStack *stack, char a){ // a za a<
	tokenparam buffer[100];
	int i;
	tokenparam LBtoken;
	LBtoken.token = LB;
	for (i = 0; top(stack) != a; i++){
		pop (stack, &buffer[i]);
	}
	push(stack, LBtoken);
	i = i - 1; // dekrementation after for cycle
	for (; i >= 0; i--){
		push(stack, buffer[i]);
	}
}

int top(tStack *stack){
	if (stack->top == -1)
		error(INTERN_ERR);
	return stack->arr[stack->top].token;
}

char topTerm(tStack *stack) { // returns terminal closes to the top of the stack

	if (stack->top == -1)
		error(INTERN_ERR);
	for (int i = stack->top; i >= 0; i--)
		if (stack->arr[i].token != 15 && stack->arr[i].token != 16 && stack->arr[i].token != 17)
		{
			return stack->arr[i].token;
		}
	error(INTERN_ERR);
	return 0;
}
//
// /Precedence stack functions
//

//
// Converts token from scanner to precedence tokens
//
int precedenceTokenConversion(char token, tokenparam *converted) //converts token code to code which can be used as an index in precedence table
{
	switch (token)
	{
	case T_ID:
		converted->token = PRID;
		return FINE;
	case L_INT:
		converted->token = PRID;
		return FINE;
	case L_FLOAT:
		converted->token = PRID;
		return FINE;
	case L_STRING:
		converted->data = NULL;
		stringCpy(converted->data, attribute->data);
		converted->token = PRID;
		return FINE;
	
	case T_LB:
		converted->token = PRLB;
		return FINE;

	case T_RB:
		converted->token = PRRB;
		return FINE;

	case T_ADD:
		converted->token = PRPLUS;
		return FINE;

	case T_SUB:
		converted->token = PRMINUS;
		return FINE;

	case T_TIMES:
		converted->token = PRTIMES;
		return FINE;

	case T_DIV:
		converted->token = PRDIV;
		return FINE;

	case T_IDIV:
		converted->token = PRIDIV;
		return FINE;

	case T_LT:
		converted->token = PRLT;
		return FINE;

	case T_GT:
		converted->token = PRGT;
		return FINE;

	case T_GTE:
		converted->token = PREGT;
		return FINE;

	case T_LTE:
		converted->token = PRELT;
		return FINE;

	case T_EQ:
		converted->token = PREQUAL;
		return FINE;

	case T_NEQ:
		converted->token = PRNEQ;
		return FINE;

	default:
		converted->token = PREND;
		return FINE;
	}
}

//
// Checks syntactical validity of expression
//
void testTokens(bool boolExpected){ // imput token string control
	bool compareTokenExists = 0; // compare token exists
	bool previousNonId = 0; // previous non-ID
	int leftBracktCounter = 0; // left brackt counter
	bool loop = true;
	for(int i = 0; loop; i++){
		token = getToken(attribute,&tokenSize);
		if (token == LEX_ERR){
			error(LEX_ERR);
		}
		precedenceBuffer[i].token = token;
		switch(precedenceBuffer[i].token){
			case T_ID:
				if (!getInitialized(*currentSymtable, attribute->data)){
					error(DEF_ERR);
				}
				precedenceBuffer[i].data = NULL;
				stringInit(&precedenceBuffer[i].data);
				stringCpy(precedenceBuffer[i].data, attribute->data);
				precedenceBuffer[i].type = getType(*currentSymtable, attribute->data);
				precedenceBuffer[i].identifier = true;
				previousNonId =	0;
				break;		
			case L_INT:
				precedenceBuffer[i].data = NULL;
				stringInit(&precedenceBuffer[i].data);
				stringCpy(precedenceBuffer[i].data, attribute->data);
				precedenceBuffer[i].type = INTEGER;
				precedenceBuffer[i].identifier = false;
				previousNonId =	0;
				break;		
			case L_FLOAT:
				precedenceBuffer[i].data = NULL;
				stringInit(&precedenceBuffer[i].data);
				stringCpy(precedenceBuffer[i].data, attribute->data);
				precedenceBuffer[i].type = DOUBLE;
				precedenceBuffer[i].identifier = false;
				previousNonId =	0;
				break;		
			case L_STRING:
				precedenceBuffer[i].data = NULL;
				stringInit(&precedenceBuffer[i].data);
				stringCpy(precedenceBuffer[i].data, attribute->data);
				precedenceBuffer[i].type = STRING;
				precedenceBuffer[i].identifier = false;
				previousNonId =	0;
				break;					
			case T_LB: // (
				leftBracktCounter++;
				break;
			case T_RB: // )
				leftBracktCounter--;
				break;				
			case T_ADD: // +
			case T_SUB: // -
			case T_TIMES: // *
			case T_DIV: // /
			case T_IDIV: 
				if (previousNonId == 0){ // no previous non-ID
					previousNonId = 1;
				}
				else if (previousNonId == 1){
					error(SYN_ERR); 
				}
				break;
			case T_LT:
			case T_GT:
			case T_GTE:
			case T_LTE:
			case T_EQ:
			case T_NEQ:
				if (!boolExpected){
					error(SEM_ERR);
				}	
				if(compareTokenExists == 0){
					compareTokenExists = 1;
					previousNonId = 0;
				}
				else{
					error(SEM_ERR); 
				}
				break;
			default:
				pushbackAttr(tokenSize);
				loop=false;
				break;
		}
	}

	if (boolExpected && !compareTokenExists){
		error(TYPE_ERR);
	}

}

//
// runs a precedence analysis of expression and generates appropriate instructions
//
void precedence_analysis(bool boolExpected){
	testTokens(boolExpected);
	tStack stack;	
	stackInit(&stack);
	tokenparam firstToken, b;
	firstToken.token = PREND;
	push(&stack, firstToken);
	char a = PREND;
	int i = 0;
	bool whileBreak=false;
	precedenceTokenConversion(precedenceBuffer[i].token, &b); 
	do {
		b.data = precedenceBuffer[i].data;
		b.type = precedenceBuffer[i].type;
		b.identifier = precedenceBuffer[i].identifier;
		switch (precTable[(int)a][(int)(b.token)]){
			case EQ:
				push(&stack, b);
				break;
			case L:
				changeA(&stack, a);
				push(&stack, b);
				break;
			case R:
				replaceY(&stack, a);
				i--;
				break;
			case E:
			default:
				if ((b.token == 14) && (a == 14)){
					whileBreak = true;
					break;
				}
				exit(100);
		}
		i++;
		a = topTerm(&stack);
		precedenceTokenConversion(precedenceBuffer[i].token, &b);
	} while ((((b.token) != 14) || (a != 14)) && !whileBreak);
	tokenparam result;
	pop(&stack, &result);
}

//
// Gets a token from scvanner that is NOT an EOL token
//
void getNEOLToken(String *s, int *size){
	while(1){
		token = getToken(s,size);
		if (token == LEX_ERR)
			error(LEX_ERR);
		if (token == T_EOL){
			continue;
		}
		break;
	}
}

//
// Gets a token from scvanner and checks if it is a specific token
//
void getNCheckToken(String *s, int t){
	if (t == T_EOL)
		token = getToken(s,&tokenSize);
	else
		getNEOLToken(s, &tokenSize);
	
	if (token == LEX_ERR)
			error(LEX_ERR);

	if (token != t){
		error(SYN_ERR);
	}
}

//
// Syntactical analysis with semantic controls and instruction generation
//
bool parse(){
	stringInit(&attribute);
	stringInit(&variableName);
	stringInit(&functionName);
	stringInit(&inFunction);
	stringInit(&operand1);
	stringInit(&operand2);
	stringInit(&operand3);
stringCpy(operand1, "SCOPE");
instruction("JUMP", operand1, NULL, NULL, NULL, NULL, NULL);
	preparePredefined(&symtable);
	writePredefined(); //zapis kodu do outu v instruction.c
	getNEOLToken(attribute, &tokenSize);
	switch(token){
		case T_DECLARE:
		case T_FUNCTION:
			functionsState();
		case T_SCOPE:
			scopeState();
			break;
		default:
			error(SYN_ERR);
		}
	getNEOLToken(attribute, &tokenSize);
	
instruction("POPFRAME", NULL, NULL, NULL, NULL, NULL, NULL);

	return (token == T_EOF);
}

void functionsState(){
	bool loop = true;
	while(loop){
		switch(token){
			case T_DECLARE:
			case T_FUNCTION:
				functionState();
				break;
			default:
				if (token != T_SCOPE){
					error(SYN_ERR);
				}
				loop = false;
				break;
		}
	}
}

void functionState(){
	switch(token){
		case T_DECLARE:
			getNCheckToken(attribute, T_FUNCTION);
			getNCheckToken(functionName, T_ID);

			insert_function(&symtable, true, functionName->data);

			getNCheckToken(attribute, T_LB);
			paramsState(true);
			getNCheckToken(attribute, T_AS);
			getNEOLToken(attribute, &tokenSize);
			typeState();

			insert_type(symtable, functionName->data, type);

			getNCheckToken(attribute, T_EOL);
			getNEOLToken(attribute, &tokenSize);
			break;
		case T_FUNCTION:
			getNCheckToken(functionName, T_ID);
			stringCpy(inFunction, functionName->data);
instruction("LABEL", inFunction, NULL, NULL, NULL, NULL, NULL);
			insert_function(&symtable, false, functionName->data);

			getNCheckToken(attribute, T_LB);
			
			paramsState(false);
			if (validateDefinitionParameters(symtable, functionName->data)){
				error(DEF_ERR);
			}

			getNCheckToken(attribute, T_AS);
			getNEOLToken(attribute, &tokenSize);
			typeState();
			int funType = type;

			insert_type(symtable, functionName->data, funType);

			currentSymtable = &nodeSearch(symtable, functionName->data)->symbol->function.functTable;
			loadParameters(symtable, functionName->data);
			fcommandsState(T_END);
			currentSymtable = &symtable;
			getNCheckToken(attribute, T_FUNCTION);
			getNCheckToken(attribute, T_EOL);
			setFunctionDefined(symtable, inFunction->data);
			switch(funType){
				case INTEGER:
stringCpy(operand1, "0");
instruction("PUSHS", operand1, NULL, NULL, "int", NULL, NULL);	
					break;
				case DOUBLE:
stringCpy(operand1, "0.0");
instruction("PUSHS", operand1, NULL, NULL, "float", NULL, NULL);	
					break;
				case STRING:
stringCpy(operand1, "");
instruction("PUSHS", operand1, NULL, NULL, "string", NULL, NULL);	
					break;
				default:
					error(SEM_ERR);
			}		
instruction("RETURN", NULL, NULL, NULL, NULL, NULL, NULL);
			getNEOLToken(attribute, &tokenSize);
			break;
		default:
			error(SYN_ERR);
			break;
	}
}

void scopeState(){
	currentSymtable = &symtable;
	getNCheckToken(attribute, T_EOL);
stringCpy(operand1, "SCOPE");
instruction("LABEL", operand1, NULL, NULL, NULL, NULL, NULL);
instruction("CREATEFRAME", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHFRAME", NULL, NULL, NULL, NULL, NULL, NULL);
stringCpy(operand1, "$AKU");
instruction("DEFVAR", operand1, NULL, NULL, "GF", NULL, NULL);
	scommandsState();
	getNCheckToken(attribute, T_SCOPE);
}

void paramsState(bool declaration){
	getNEOLToken(variableName, &tokenSize);
	switch(token){
		case T_ID:
			paramState(declaration);
			nparamState(declaration);
			break;
		case T_RB:
			break;
		default:
			error(SYN_ERR);
			break;
	}
}

void paramState(bool declaration){
	if (!declaration){
instruction("DEFVAR", variableName, NULL, NULL, "LF", NULL, NULL);
	}
	getNCheckToken(attribute, T_AS);
	getNEOLToken(attribute, &tokenSize);
	typeState();

	insert_param(symtable, functionName->data, variableName->data, type, declaration);
}

void nparamState(bool declaration){
	bool loop = true;
	while (loop){
		getNEOLToken(attribute, &tokenSize);
		switch(token){
			case T_COMMA:
				getNCheckToken(variableName, T_ID);
				paramState(declaration);
				break;
			case T_RB:
				loop = false;
				break;
			default:
				error(SYN_ERR);
		}
	}
}

void fcommandsState(int finalizingToken){
	bool loop = true;
	while(loop){
		getNEOLToken(attribute, &tokenSize);
		switch (token){
			case T_RETURN:
				fcommandState();
				break;
			case T_DIM:
				scommandState();
				break;
			case T_ID:
			case T_INPUT:
			case T_PRINT:
			case T_IF:
			case T_DO:
				commandState(true);
				break;
			default:
				if (token == finalizingToken){
					loop = false;
					break;
				}
				else{
					error(SYN_ERR);
				}
		}
	}
}

void fcommandState(){
	getNEOLToken(attribute, &tokenSize);
	if (token != T_LB && token != T_ID && token != L_INT && token != L_STRING && token != L_FLOAT){
		error(SYN_ERR);
	}
	pushbackAttr(tokenSize);
	precedence_analysis(false);
	if (expressionType != nodeSearch(symtable, inFunction->data)->symbol->type)
		error(TYPE_ERR);
	getNCheckToken(attribute, T_EOL);
instruction("RETURN", NULL, NULL, NULL, NULL, NULL, NULL);
	set_hasReturn(symtable, inFunction->data);
}

void scommandsState(){
	bool loop = true;
	while(loop){
		getNEOLToken(attribute, &tokenSize);
		switch (token){
			case T_DIM:
				scommandState();
				break;
			case T_ID:
			case T_INPUT:
			case T_PRINT:
			case T_IF:
			case T_DO:
				commandState(false);
				break;
			case T_END:
				loop = false;
				break;
			default:
				error(SYN_ERR);
		}
	}
}

void scommandState(){
	vardefState();
}

void vardefState(){
	getNCheckToken(variableName, T_ID);
	insert_variable(currentSymtable, variableName->data);
instruction("DEFVAR", variableName, NULL, NULL, "LF", NULL, NULL);
	getNCheckToken(attribute, T_AS);
	getNEOLToken(attribute, &tokenSize);
	typeState();
	insert_type(*currentSymtable, variableName->data, type);
	set_initialized(*currentSymtable,variableName->data);
	definitState();
}

void definitState(){
	token = getToken(attribute, &tokenSize);
	if (token == LEX_ERR){
		error(LEX_ERR);
	}
	switch(token){
		case T_EQ:
			initState();
			break;
		case T_EOL:
			break;
		default:
			error(SYN_ERR);
	}
}

void initState(){
	String *storedAttr = NULL;
	stringInit(&storedAttr);
	int storedSize;
	getNEOLToken(storedAttr, &storedSize);
	switch(token){
		case T_ID:
			token = getToken(attribute, &tokenSize);
			if (token == LEX_ERR) error(LEX_ERR);
			if (token == T_LB){
				stringCpy(functionName, storedAttr->data);
				fcallState();
				break;
			}
			else{
				pushbackAttr(tokenSize);
			}
		case L_INT:
		case L_FLOAT:
		case L_STRING:
		case T_LB:
			pushbackAttr(storedSize);
			precedence_analysis(false);
			if (getType(*currentSymtable, variableName->data) != expressionType)
				error(TYPE_ERR);
if (type == INTEGER && expressionType == DOUBLE){
instruction("FLOAT2R2EINTS", NULL, NULL, NULL, NULL, NULL, NULL);
}
else if (type == DOUBLE && expressionType == INTEGER){
instruction("INT2FLOATS", NULL, NULL, NULL, NULL, NULL, NULL);	
} 
			//insert_value(*currentSymtable, variableName->data, type, precedenceResult, expressionType);
			break;
		default:
			error(SYN_ERR);
	}
stringCpy(operand1, variableName->data);
instruction("POPS", operand1, NULL, NULL, "LF", NULL, NULL);
}

void fcallState(){
	validateFunctCall(symtable, *currentSymtable, variableName->data, functionName->data);
	// String *variable = NULL;
	// stringInit(&variable);
	// stringCpy(variable, variableName->data);

	cparamsState();
instruction("CREATEFRAME", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("PUSHFRAME", NULL, NULL, NULL, NULL, NULL, NULL);
instruction("CALL", functionName, NULL, NULL, NULL, NULL, NULL);
instruction("POPFRAME", NULL, NULL, NULL, NULL, NULL, NULL);
}

//
// Helper function that inserts a parameter into a list of parameters in function call
//
void addParamToList(param *paramList, int t){
	param *head = paramList;

	param newParam = saveMalloc(sizeof(struct parameters));
	newParam->type = t;

	if(*head == NULL) {
		*head = newParam;
	}	
	else {
		param current = *head;
		while(current->next != NULL) {		
			current = current->next;
		}
		current->next = newParam;	
	}
}

void cparamsState(){
	param callParameters = NULL;
	getNEOLToken(attribute, &tokenSize);
	switch(token){
		case T_RB:
			break;
		default:
			cparamState(&callParameters);
			ncparamState(&callParameters);
			break;
	}

	if (validateCallParams(symtable, functionName->data, callParameters)){
		error(TYPE_ERR);
	}
}

void cparamState(param *callParameters){
	termState();
	addParamToList(callParameters, type);
}

void ncparamState(param *callParameters){
	bool loop = true;
	while(loop){
		getNEOLToken(attribute, &tokenSize);
		switch(token){
			case T_COMMA:
				token = getToken(attribute, &tokenSize);
				if (token == LEX_ERR){
					error(LEX_ERR);
				}
				cparamState(callParameters);
				break;
			case T_RB:
				loop = false;
				break;
			default:
				error(SYN_ERR);
		}
	}
}

void commandsState(int finalizingToken){
	bool loop = true;
	while(loop){
		getNEOLToken(attribute, &tokenSize);
		switch(token){
			case T_ID:
			case T_INPUT:
			case T_PRINT:
			case T_IF:
			case T_DO:
				commandState(false);
				break;
			default:
				if (token == finalizingToken){
					loop = false;
					break;
				}
				else{
					error(SYN_ERR);
				}
		}
	}
}

void commandState(bool fBody){
	switch(token){
		case T_ID:
			stringCpy(variableName, attribute->data);
			getNCheckToken(attribute, T_EQ);
			initState();
			getNCheckToken(attribute, T_EOL);
			break;
		case T_INPUT:
			inputState();
			break;
		case T_PRINT:
			printState();
			break;
		case T_IF:
			branchState(fBody);
			break;
		case T_DO:
			loopState(fBody);
			break;
		default:
			error(SYN_ERR);
	}
}

void inputState(){
	getNCheckToken(attribute, T_ID);

switch (getType(*currentSymtable, attribute->data)){
	case INTEGER:
		stringCpy(operand2, "int");
		break;
	case DOUBLE:
		stringCpy(operand2, "float");
		break;
	case STRING:
		stringCpy(operand2, "string");
		break;
	default:
		error(INTERN_ERR);
}
instruction("READ", attribute, operand2, NULL, "LF", NULL, NULL);

	if (nodeSearch(*currentSymtable, attribute->data) == NULL){
		error(DEF_ERR);
	}
	set_initialized(*currentSymtable, attribute->data);
	getNCheckToken(attribute, T_EOL);
}

void printState(){
	getNEOLToken(attribute, &tokenSize);
	if (token != T_ID && token != L_INT && token != L_STRING && token != L_FLOAT){
		error(SYN_ERR);
	}
	pushbackAttr(tokenSize);
	precedence_analysis(false);
	getNCheckToken(attribute, T_SEMICOLON);
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("WRITE", operand1, NULL, NULL, "GF", NULL, NULL);
	nexprState();
}

void nexprState(){
	bool loop = true;
	while(loop){
		token = getToken(attribute, &tokenSize);
		if (token == LEX_ERR){
			error(LEX_ERR);
		}
		switch(token){
			case T_ID:
			case L_INT:
			case L_FLOAT:
			case L_STRING:
			case T_LB:
				pushbackAttr(tokenSize);
				precedence_analysis(false);
				getNCheckToken(attribute, T_SEMICOLON);	
stringCpy(operand1, "$AKU");
instruction("POPS", operand1, NULL, NULL, "GF", NULL, NULL);
instruction("WRITE", operand1, NULL, NULL, "GF", NULL, NULL);
				break;			
			case T_EOL:
				loop = false;
				break;
			
			default:
				error(SYN_ERR);
				break;
		}
	}
}

void branchState(bool fBody){
	ifCounter++;
	char toAppend[64];
	snprintf(toAppend, 64, "%d", ifCounter);
	getNEOLToken(attribute, &tokenSize);
	if (token != T_LB && token != T_ID && token != L_INT && token != L_STRING && token != L_FLOAT){
		error(SYN_ERR);
	}
	pushbackAttr(tokenSize);
	precedence_analysis(true);

stringCpy(operand1, "true");
instruction("PUSHS", operand1, NULL, NULL, "bool", NULL, NULL);

stringCpy(operand1, "ELSE");
concatToString(operand1, toAppend);
instruction("JUMPIFNEQS", operand1, NULL, NULL, NULL, NULL, NULL);

	getNCheckToken(attribute, T_THEN);
	getNCheckToken(attribute, T_EOL);
	if (fBody){
		fcommandsState(T_ELSE);
	}
	else
	{
		commandsState(T_ELSE);
	}
	getNCheckToken(attribute, T_EOL);

stringCpy(operand1, "ENDIF");
concatToString(operand1, toAppend);
instruction("JUMP", operand1, NULL, NULL, NULL, NULL, NULL);

stringCpy(operand1, "ELSE");
concatToString(operand1, toAppend);
instruction("LABEL", operand1, NULL, NULL, NULL, NULL, NULL);

	commandsState(T_END);
	getNCheckToken(attribute, T_IF);
	getNCheckToken(attribute, T_EOL);

stringCpy(operand1, "ENDIF");
concatToString(operand1, toAppend);
instruction("LABEL", operand1, NULL, NULL, NULL, NULL, NULL);
}

void loopState(bool fBody){	
	loopCounter++;
	char toAppend[64];
	snprintf(toAppend, 64, "%d", loopCounter);
	getNCheckToken(attribute, T_WHILE);

stringCpy(operand1, "WHILESTART");
concatToString(operand1, toAppend);
instruction("LABEL", operand1, NULL, NULL, NULL, NULL, NULL);

	getNEOLToken(attribute, &tokenSize);
	if (token != T_LB && token != T_ID && token != L_INT && token != L_STRING && token != L_FLOAT){
		error(SYN_ERR);
	}
	pushbackAttr(tokenSize);
	precedence_analysis(true);
	getNCheckToken(attribute, T_EOL);

stringCpy(operand1, "true");
instruction("PUSHS", operand1, NULL, NULL, "bool", NULL, NULL);

stringCpy(operand1, "WHILEEND");
concatToString(operand1, toAppend);
instruction("JUMPIFNEQS", operand1, NULL, NULL, NULL, NULL, NULL);

	if (fBody){
		fcommandsState(T_LOOP);
	}
	else
	{
		commandsState(T_LOOP);
	}
	getNCheckToken(attribute, T_EOL);


stringCpy(operand1, "WHILESTART");
concatToString(operand1, toAppend);
instruction("JUMP", operand1, NULL, NULL, NULL, NULL, NULL);

stringCpy(operand1, "WHILEEND");
concatToString(operand1, toAppend);
instruction("LABEL", operand1, NULL, NULL, NULL, NULL, NULL);
}

void typeState(){
	switch(token){
		case T_INTEGER:
			type = INTEGER;
			break;
		case T_DOUBLE:
			type = DOUBLE;
			break;
		case T_STRING:
			type = STRING;
			break;
		default:
			error(SYN_ERR);
	}
}

void termState(){
	switch(token){
		case L_INT:
instruction("PUSHS", attribute, NULL, NULL, "int", NULL, NULL);
			type = INTEGER; 
			break;
		case L_FLOAT:
instruction("PUSHS", attribute, NULL, NULL, "float", NULL, NULL);
			type = DOUBLE;
			break;
		case L_STRING:
instruction("PUSHS", attribute, NULL, NULL, "string", NULL, NULL);
			type = STRING;
			break;
		case T_ID:
instruction("PUSHS", attribute, NULL, NULL, "LF", NULL, NULL);
			type = nodeSearch(*currentSymtable, attribute->data)->symbol->type; 
			break;
		default:
			error(SYN_ERR);
	}
}