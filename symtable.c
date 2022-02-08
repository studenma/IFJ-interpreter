/*
* 	Project 		Implementace překladače imperativního jazyka IFJ17.
 *	File			symtable.c
 *	Description		Source file for binary search tree and semantic functions
 *	Author			Martin Studeny (xstude23), Jakub Zapletal(xzaple36)
 */
#include "symtable.h"

//
// Binary tree functions
//
void treeInit(nodePtr *tree) {
    *tree = NULL;
}

void treeDispose(nodePtr *tree) {
	if( (*tree) != NULL) {
		treeDispose(&(*tree)->lPtr);
		treeDispose(&(*tree)->rPtr);
	}
	*tree = NULL;
}

nodePtr nodeSearch(nodePtr tree, char* K) {
	if(tree != NULL) {
		if(strcmp(tree->key, K) == 0) {
			//*content = tree->symbol;
			return tree;
		}
		else if(strcmp(tree->key, K) > 0)
			return nodeSearch(tree->lPtr, K/*,content*/);
		else
			return nodeSearch(tree->rPtr, K/*,content*/);
	}
	else
		return NULL;
}

void nodeInsert(nodePtr* tree, loadPtr load, char* K) {
	if(*tree == NULL) {
		*tree = saveMalloc(sizeof(struct node));
		strcpy((*tree)->key, K);
		(*tree)->lPtr = NULL;
		(*tree)->rPtr = NULL;
		(*tree)->symbol = load;
	}
	else {
		if(strcmp((*tree)->key, K) == 0) {
			(*tree)->symbol = load;
			return;
		}
		else {
			if(strcmp((*tree)->key, K) > 0)
				nodeInsert(&(*tree)->lPtr, load, K);
			else
				nodeInsert(&(*tree)->rPtr, load, K);
		}
	}
}

void ReplaceByRightmost(nodePtr treeReplaced, nodePtr *tree) {
	if((*tree)->rPtr != NULL) {
		ReplaceByRightmost(treeReplaced, &(*tree)->rPtr);
		return;
	}
	else {
		treeReplaced->symbol = (*tree)->symbol;
		strcpy(treeReplaced->key,(*tree)->key);
		*tree = (*tree)->lPtr;
	}
}

void nodeDelete(nodePtr *tree, char *K) {
	if(*tree == NULL)
		return;
	if(strcmp((*tree)->key, K) == 0) {
		if((*tree)->lPtr != NULL && (*tree)->rPtr != NULL) {
			ReplaceByRightmost(*tree, &(*tree)->lPtr);
			return;
		}
		if((*tree)->lPtr != NULL && (*tree)->rPtr == NULL) {
			*tree = (*tree)->lPtr;
			return;
		}
		if((*tree)->lPtr == NULL && (*tree)->rPtr != NULL) {
			*tree = (*tree)->rPtr;
			return;
		}
		if((*tree)->lPtr == NULL && (*tree)->rPtr == NULL) {
			*tree = NULL;
			return;
		}
	}
	else {
		if(strcmp((*tree)->key, K) > 0)
			nodeDelete(&(*tree)->lPtr, K);
		else
			nodeDelete(&(*tree)->rPtr, K);
	}
}
//
// /Binary tree functions
//


//
// Add variable symbol into tree
//
void insert_variable(nodePtr *Strom, char *name) {
	nodePtr uzel;

	loadPtr Content_of_Insert = saveMalloc(sizeof(struct load));
	//val tmptmp = malloc(sizeof(struct value));
	//strcpy(tmp, name);
	//generateKey(tmp,1);

	uzel = nodeSearch(*Strom, name);
	if(uzel != NULL) {
		error(DEF_ERR);
	}
	else {
		Content_of_Insert->defined = 1;
		Content_of_Insert->metaType = VARIABLE;
		stringInit(&(Content_of_Insert->value.s));
		nodeInsert(Strom, Content_of_Insert, name);
	}
}

//
// sets symbol type
//
void insert_type(nodePtr Strom, char *name, int type) {
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	if (uzel->symbol->metaType == FUNCTION){
		if (uzel->symbol->function.declared && uzel->symbol->type != type){
			error(DEF_ERR);
		}
	}
	else{
		String *operand = NULL; 
		stringInit(&operand);
		String *operand2 = NULL; 
		stringInit(&operand2);
		switch(type){
			case INTEGER:
stringCpy(operand, name);
stringCpy(operand2, "0");
instruction("MOVE", operand, operand2, NULL, "LF", "int", NULL);
				break;
			case DOUBLE:
stringCpy(operand, name);
stringCpy(operand2, "0.0");
instruction("MOVE", operand, operand2, NULL, "LF", "float", NULL);
				break;
			case STRING:
stringCpy(operand, name);
stringCpy(operand2, "");
instruction("MOVE", operand, operand2, NULL, "LF", "string", NULL);
				break;
		}
	}

	uzel->symbol->type = type;
}

//
// sets symbol value
//
void insert_value(nodePtr Strom, char *name, int type, val data, int valueType) {
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if(uzel == NULL) {
		error(DEF_ERR);
	}
	switch(type){
		case INTEGER:
			if (valueType == INTEGER)
				uzel->symbol->value.i = data.i;
			else if (valueType == DOUBLE){
				uzel->symbol->value.i = data.d;
			}
			else{
				error(TYPE_ERR);
			}
			break;
		case DOUBLE:
			if (valueType == INTEGER){

				uzel->symbol->value.d = data.i;
			}
			else if (valueType == DOUBLE)
				uzel->symbol->value.d = data.d;
			else{
				error(TYPE_ERR);
			}
			break;
		case STRING:
			if (valueType != STRING){
				error(TYPE_ERR);
			}
			stringInit(&uzel->symbol->value.s);
			concatToString(uzel->symbol->value.s, data.s->data);
			break;
		default:
			error(INTERN_ERR);
			break;
	}
	uzel->symbol->initialized = true;
}

//
// Add function symbol into tree and sets it to declared or defined, depending on given parameters
//
void insert_function(nodePtr *Strom, bool declaration, char *name) {
	nodePtr uzel;
	uzel = nodeSearch(*Strom, name);


	if(declaration == true) {
		if(uzel != NULL) {
			error(DEF_ERR);
		}
		else {
			loadPtr Content_of_Insert = saveMalloc(sizeof(struct load));
			Content_of_Insert->metaType = FUNCTION;
			Content_of_Insert->defined = false;
			Content_of_Insert->function.declared = true;
			Content_of_Insert->function.hasReturn = false;
			treeInit(&Content_of_Insert->function.functTable);

			nodeInsert(Strom, Content_of_Insert, name);
		}
	}
	else {
		if(uzel == NULL) {
			loadPtr Content_of_Insert = saveMalloc(sizeof(struct load));
			Content_of_Insert->metaType = FUNCTION;
			Content_of_Insert->function.declared = false;
			Content_of_Insert->function.hasReturn = false;
			treeInit(&Content_of_Insert->function.functTable);

			nodeInsert(Strom, Content_of_Insert, name);
		}
		else if(uzel->symbol->defined == 1) {
			error(DEF_ERR);
		}
	}
}

//
// Set function as defined
//
void setFunctionDefined(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	uzel->symbol->defined = true;
}

//
// Set function as a function with return clause
//
void set_hasReturn(nodePtr Strom, char *name) {
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(INTERN_ERR);
	}

	uzel->symbol->function.hasReturn = true;
}


//
// Creates a local variable in functions symtable for a parameter
//
void insert_param(nodePtr Strom, char *name, char *parName, int type, bool declaration) {
	nodePtr function = nodeSearch(Strom, name);
	
	param newParam = saveMalloc(sizeof(struct parameters));
	newParam->type = type;
	strcpy(newParam->name, parName); 

	if (declaration){
		param *head = &function->symbol->function.declaredParameters;
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
	else{
		param *head = &function->symbol->function.parameters;
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
		nodePtr uzel = nodeSearch(Strom, parName);
		if(uzel != NULL) {
			if (uzel->symbol->metaType == FUNCTION)
				error(DEF_ERR);
		}
		insert_variable(&function->symbol->function.functTable, parName);
		insert_type(function->symbol->function.functTable, parName, type);
	}
}


//
// Check if parameters in definition match those in declaration
//
int validateDefinitionParameters(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);	

	if (!uzel->symbol->function.declared)
		return FINE;
	
	param headDec = uzel->symbol->function.declaredParameters;
	param headDef = uzel->symbol->function.parameters;

	if (headDec == NULL && headDef == NULL)
		return FINE;

	if ((headDec == NULL && headDef != NULL) || (headDec != NULL && headDef == NULL))
		return FAIL;

	while (headDec != NULL && headDef != NULL){
		if (headDec->type != headDef->type)
			return FAIL;
		
		headDec = headDec->next;
		headDef = headDef->next;
	}

	if ((headDec == NULL && headDef != NULL) || (headDec != NULL && headDef == NULL))
		return FAIL;

	return FINE;
}


//
// Check if type of variable and function match in call
//
void validateFunctCall(nodePtr Strom, nodePtr lokalniStrom, char *varName, char *functName){
	nodePtr function = nodeSearch(Strom, functName);
	nodePtr variable = nodeSearch(lokalniStrom, varName);

	if (variable == NULL){
		error(DEF_ERR);
		exit(-1);
	}
	
	if (function == NULL){
		error(DEF_ERR);
		exit(-2);
	}

	if (!function->symbol->defined && !function->symbol->function.declared){
		error(DEF_ERR);
		exit(-3);
	}

	if ((function->symbol->type == STRING && variable->symbol->type != STRING) || (function->symbol->type != STRING && variable->symbol->type == STRING)){
		error(TYPE_ERR);
	}
}

//
// Check if parameters in call match those in declaration or definition
//
int validateCallParams(nodePtr Strom, char *name, param callParams){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	param headDef;
	
	if (uzel->symbol->defined)
		headDef = uzel->symbol->function.parameters;
	else 
		headDef = uzel->symbol->function.declaredParameters;
		
	param headCall = callParams;

	if (headDef == NULL && headCall == NULL)
		return FINE;

	if ((headDef == NULL && headCall != NULL) || (headDef != NULL && headCall == NULL))
		return FAIL;

	while (headDef != NULL && headCall != NULL){
		if (headDef->type != headCall->type)
			return FAIL;
		
		headDef = headDef->next;
		headCall = headCall->next;
	}

	if ((headCall == NULL && headDef != NULL) || (headCall != NULL && headDef == NULL))
		return FAIL;

	return FINE;

}

//
// Sets variable as initialized
//
void set_initialized(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	uzel->symbol->initialized = true;
}

//
// returns true if variable is initialized
//
bool getInitialized(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	return uzel->symbol->initialized;
}

//
// returns value of variable
//
val getValue(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	return uzel->symbol->value;
}

//
// returns type of variable
//
int getType(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	return uzel->symbol->type;
}

//
// generates instructions for loading parameter values in function call
//
void loadParameters(nodePtr Strom, char *name){
	nodePtr uzel;
	uzel = nodeSearch(Strom, name);

	if (uzel == NULL){
		error(DEF_ERR);
	}

	param currentParameter = uzel->symbol->function.parameters;
	
	if (currentParameter == NULL){
		return;
	}
	
	param lastParameter = NULL;
	String *operand = NULL;
	stringInit(&operand);


	do{
		while (currentParameter->next != lastParameter){
			currentParameter = currentParameter->next;			
		}
stringCpy(operand, currentParameter->name);
instruction("POPS", operand, NULL, NULL, "LF", NULL, NULL);
		set_initialized(uzel->symbol->function.functTable, currentParameter->name);
		lastParameter = currentParameter;
		currentParameter = uzel->symbol->function.parameters;
	}while (lastParameter != uzel->symbol->function.parameters);
}

//
// inserts predefined functions into symtable
//
void preparePredefined(nodePtr *Strom){
	int type;

	String *functionName = NULL;
	String *temp1 = NULL;
	String *temp2 = NULL;
	String *param1 = NULL;
	String *param2 = NULL;
	String *param3 = NULL;

	stringInit(&functionName);
	stringInit(&temp1);
	stringInit(&temp2);
	stringInit(&param1);
	stringInit(&param2);
	stringInit(&param3);

	/* Length */
	stringCpy(functionName, "length");
	stringCpy(param1, "s");
	type = INTEGER;

	insert_function(Strom, false, functionName->data);
	insert_param(*Strom, functionName->data, param1->data, STRING, false);
	insert_type(*Strom, functionName->data, type);
	setFunctionDefined(*Strom, functionName->data);
	/* /Length */

	/* SubStr */
	stringCpy(functionName, "substr");
	stringCpy(param1, "s");
	stringCpy(param2, "i");
	stringCpy(param3, "n");
	type = STRING;

	insert_function(Strom, false, functionName->data);
	insert_param(*Strom, functionName->data, param1->data, STRING, false);
	insert_param(*Strom, functionName->data, param2->data, INTEGER, false);
	insert_param(*Strom, functionName->data, param3->data, INTEGER, false);
	insert_type(*Strom, functionName->data, type);
	setFunctionDefined(*Strom, functionName->data);
	/* /SubStr */

	/* asc */
	stringCpy(functionName, "asc");
	stringCpy(param1, "s");
	stringCpy(param2, "i");
	type = INTEGER;

	insert_function(Strom, false, functionName->data);
	insert_param(*Strom, functionName->data, param1->data, STRING, false);
	insert_param(*Strom, functionName->data, param2->data, INTEGER, false);
	insert_type(*Strom, functionName->data, type);
	setFunctionDefined(*Strom, functionName->data);
	/* /asc */

	/* Chr */
	stringCpy(functionName, "chr");
	stringCpy(param1, "i");
	type = STRING;

	insert_function(Strom, false, functionName->data);
	insert_param(*Strom, functionName->data, param1->data, INTEGER, false);
	insert_type(*Strom, functionName->data, type);
	setFunctionDefined(*Strom, functionName->data);
	/* /Chr */



}