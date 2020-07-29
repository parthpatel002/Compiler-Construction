/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef NARYTREEDEF
#define NARYTREEDEF

#include "lexerDef.h"
#include "parserDef.h"
#include "hashTableDef.h"

typedef enum{
	primitiveDatatype,
	relationalOp,
	assignmentStmt,
	fieldDefinitions,
	declaration,
	moreFields,
	singleOrRecId,
	elsePart,
	output_par,
	arithmeticExpression,
	mainFunction,
	remaining_list,
	more_ids,
	returnStmt,
	idList,
	ioStmt,
	global_or_not,
	conditionalStmt,
	stmts,
	input_par,
	allVar,
	program,
	booleanExpression,
	highPrecedenceOperators,
	inputParameters,
	fieldDefinition,
	iterativeStmt,
	function,
	all,
	factor,
	logicalOp,
	dataType,
	outputParameters,
	typeDefinition,
	stmt,
	otherStmts,
	var,
	constructedDatatype,
	optionalReturn,
	termPrime,
	term,
	lowPrecedenceOperators,
	temp,
	parameter_list,
	new_24,
	declarations,
	funCallStmt,
	typeDefinitions,
	otherFunctions,
	expPrime,
	var_mid
} NonTerminal;

typedef union {
	Tokentype terminalType;
	NonTerminal nonTerminalType;
} symbolType;

struct parseTreeNode{
    int isTerminal;
    symbolType symType;
    struct parseTreeNode* nextSibling;
    Token* token;
    struct parseTreeNode* parent;
    int no_children;
    struct parseTreeNode* children;
};

typedef struct parseTreeNode parseTreeNode;


#endif