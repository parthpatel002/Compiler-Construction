/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "ast.h"
#include "astDef.h"

int toPrint = 1;

int AST_SIZE = 0;

char labelToString[][30] = {"PROGRAM_NODE", "OTHERFUNCTIONS_NODE","MAINFUNCTION_NODE","FUNCTIONDEF_NODE","INPUTPAR_NODE","OUTPUTPAR_NODE","ID_NODE","INT_NODE","REAL_NODE","RECORDID_NODE","TYPEDEFINITIONS_NODE","DECLARATIONS_NODE","OTHERSTATEMENTS_NODE","FIELDID_NODE","GLOBALID_NODE","ASSIGNOP_NODE","CALL_NODE","FUNCTIONCALL_NODE","OUTPUTPARAMETERS_NODE","INPUTPARAMETERS_NODE","WHILE_NODE","IF_NODE","ELSE_NODE","IOREAD_NODE","IOWRITE_NODE"," NUM_NODE","RNUM_NODE","MULOP_NODE","DIVOP_NODE","PLUSOP_NODE","MINUSOP_NODE","NOTOP_NODE","LTOP_NODE","GTOP_NODE","LEOP_NODE","GEOP_NODE","EQOP_NODE","NEOP_NODE","ANDOP_NODE","OROP_NODE","RETURN_NODE"};


/*used to increase no of ast nodes by 1*/
void increase_ast_size()
{
	AST_SIZE += 1;
}

/*allocates ast nodes*/
astNode* allocate_ast_node(astNode* temp,Label label, astNode* parent, astNode* first_kid, astNode* next, Token* token)
{
		astNode* node = (astNode*) malloc(sizeof(astNode));
		node->label = label;
		node->parent = parent;
		node->first_kid = first_kid;
		node->next = next;
		node->token = token;
		return node;	
}



/*
Create AST Node from given attributes
(Here, Label identifies the AST Node. For example, NUM_NODE, RNUM_NODE, RECORD_NODE etc)
Stores the parent as well as first child for debugging and printing
Token is the Lexical Token
*/
astNode* make_ast_node(astNode* temp,Label label, astNode* parent, astNode* first_kid, astNode* next, Token* token) {
	if(temp==NULL)
	{
		astNode* node = allocate_ast_node(temp,label,parent,first_kid,next,token);
		increase_ast_size();
		return node;
	}
	else 
		return NULL;
}

/* 

Function to return last AST Node in sibling list of given node.
Used to increment AST further

*/
astNode* get_last_node(astNode* node){
	while(node->next != NULL){
		node = node->next;
	}
	return node;
}

/*

Get child of a parse tree node corresponding to a given search key.

*/
parseTreeNode* obtain_first_kid(parseTreeNode* parseNode,int searchKey, int isSearchForTerminal){
	parseNode = parseNode->first_kid;
	if(isSearchForTerminal) {
		while(parseNode != NULL) {
			if(!parseNode->isTerminal)
				parseNode = parseNode->nextSibling;
			else if(parseNode->symType.terminalType == searchKey)
				break;
			else 
				parseNode = parseNode->nextSibling;
		}
	}
	else {
		while(parseNode != NULL) {
			if(parseNode->isTerminal)
				parseNode = parseNode->nextSibling;
			else if(parseNode->symType.nonTerminalType == searchKey)
				break;
			else 
				parseNode = parseNode->nextSibling;
		}
	}

	return parseNode;
}

void manage_term_values(astNode** main, astNode** ptr1_temporary, astNode** ptr2_temporary) {
	if(*ptr1_temporary != NULL){
		*ptr2_temporary = (*ptr1_temporary)->first_kid;
		(*ptr1_temporary)->first_kid = *main;
		(*ptr1_temporary)->first_kid->next = *ptr2_temporary;
		(*ptr1_temporary)->first_kid->parent = *ptr1_temporary;
		(*ptr1_temporary)->first_kid->next->parent = *ptr1_temporary;
		while((*ptr1_temporary) -> parent!=NULL){
			*ptr1_temporary = (*ptr1_temporary)->parent;
		}
		*main = *ptr1_temporary;
	}
}

void manage_term_prime(astNode** main, astNode** ptr1_temporary, astNode** ptr2_temporary) {
	*ptr2_temporary = (*ptr1_temporary)->first_kid;
	(*main)->parent = *ptr1_temporary;
	(*ptr1_temporary)->first_kid = *main;
	(*ptr2_temporary)->parent = *ptr1_temporary;
	(*ptr1_temporary)->first_kid->next = *ptr2_temporary;
	*main = (*ptr1_temporary)->first_kid;
}

float return_mul(int number) {
	return 1.15*number;
}

/*

Main function of the file, populates AST after selection of nodes from the Parse Tree and Semantic Rules

*/

astNode* populate_ast(parseTreeNode* parseTree, astNode* parent) {
	int rule_no = parseTree->rule_no;
	astNode *main,*ptr1_temporary,*ptr2_temporary;
	switch(rule_no){

		// Handle epsilon rules

		case 25:	// declarations ===> eps
		case 3:	// otherFunctions ===> eps 
		case 23: 	// moreFields ===> eps
		case 70:	// all ===> TK_ID temp
		case 57:	// expPrime ===> eps
		case 91:	// more_ids ===> eps 
		case 52:	// var_mid ===> eps
		case 7: 	// output_par ===> eps
		case 15: 	// remaining_list ===> eps
		case 18: 	// typeDefinitions ===> epstypeDefinitions ===> eps
		case 28: 	// global_or_not ===> eps
		case 30: 	// otherStmts ===> eps
		case 38: 	// new_24 ===> eps
		case 60: 	// termPrime ===> eps
		case 88:	// optionalReturn ===> eps 
		
		// Rule of the form non_terminal ===> TERMINAL not required in AST creation
		case 47: 	// elsePart ===> TK_ENDIF
			main = NULL;
			break;
		
		case 14:	// remaining_list ===> TK_COMMA parameter_list
				// ignore TK_COMMA and continue on the parameter_list
			main = populate_ast(obtain_first_kid(parseTree,parameter_list, 0), parent);
			break;

		case 33:	 // stmt ===> conditionalStmt
				 // continue normally on the child
 			main = populate_ast(obtain_first_kid(parseTree,conditionalStmt, 0),parent);
			break;

		case 51:	//  var_mid ===> TK_DOT TK_FIELDID
				// ignore TK_DOT, create node for TK_FIELDID
			main = make_ast_node(NULL,FIELDID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_FIELDID, 1)->token);
			break;

		case 44:	//  iterativeStmt ===> TK_WHILE TK_OP booleanExpression TK_CL stmt otherStmts TK_ENDWHILE
				// create node for TK_WHILE, make booleanExpression as left most child, continue for other statements
			main = make_ast_node(NULL,WHILE_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_WHILE, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,booleanExpression, 0), main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,stmt, 0), main);
			main->first_kid->next->next = populate_ast(obtain_first_kid(parseTree,otherStmts, 0), main);
			break;

		case 31:	 // stmt ===> assignmentStmt
				 // continue populating
			main = populate_ast(obtain_first_kid(parseTree,assignmentStmt, 0),parent);
			break;

		case 24:	// declarations ===> declaration declarations
				// call recursively for both first_kid
			main = populate_ast(obtain_first_kid(parseTree,declaration, 0),parent);
			main->next = populate_ast(obtain_first_kid(parseTree,declarations, 0),parent);
			break;

		case 41:	// outputParameters ===> TK_SQL idList TK_SQR TK_ASSIGNOP
				// create reference NODE for output parameters, populate with first_kid IDs 
			main = make_ast_node(NULL,OUTPUTPARAMETERS_NODE, parent, NULL, NULL, parseTree->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,idList, 0),main);
			break;

		case 48:	// ioStmt ===> TK_READ TK_OP singleOrRecId TK_CL TK_SEM
				// create reference for IOREAD, add NODE for singleOrRecID for calling populate_ast recursively
			main = make_ast_node(NULL,IOREAD_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_READ, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree, singleOrRecId, 0),main);
			break;
		
		case 29:	// otherStmts ===> stmt otherStmts
				// call for both non terminals on RHS
			main = populate_ast(obtain_first_kid(parseTree, stmt, 0),parent);
			main->next = populate_ast(obtain_first_kid(parseTree,otherStmts, 0),parent);
			break;

		case 89:	 // idList ===> TK_ID more_ids
				 // create ID Node using its token, call for more_ids
			main = make_ast_node(NULL,ID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			main->next =  populate_ast(obtain_first_kid(parseTree,more_ids, 0),parent);
			break;

		case 19:	// typeDefinition ===> TK_RECORD TK_RECORDID fieldDefinitions TK_ENDRECORD TK_SEM
				// create node for identifier, call on fieldDefinitions for corresponding ID Nodes
			main = make_ast_node(NULL,RECORDID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RECORDID, 1)->token);
            	main->first_kid = populate_ast(obtain_first_kid(parseTree, fieldDefinitions, 0),main);
            	break;
	
		case 87:	// optionalReturn ===> TK_SQL idList TK_SQR
				// ignore TK_SQL and TK_SQR, call only for idList
			main = populate_ast(obtain_first_kid(parseTree,idList, 0),parent);
			break;
	
		case 72:	// booleanExpression ===> TK_OP booleanExpression TK_CL logicalOp TK_OP booleanExpression TK_CL
				//	logicalOp
				//	  '
				//     '
				//    '
				//  booleanExpression ------> booleanExpression 
			main = populate_ast(obtain_first_kid(parseTree,logicalOp, 0),parent);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,booleanExpression, 0),main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,booleanExpression, 0)->nextSibling->nextSibling->nextSibling->nextSibling,main);
			break;
		
		case 63:	// highPrecedenceOperators ===> TK_MUL
				// create node for Op 
			main = make_ast_node(NULL,MULOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_MUL, 1)->token);
			break;

		case 79:	// logicalOp ===> TK_OR
				// Node for OR
			main = make_ast_node(NULL,OROP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_OR, 1)->token);
			break;
		
		case 58:	// term ===> factor termPrime
				// 
			main = populate_ast(obtain_first_kid(parseTree,factor, 0),parent);
			ptr1_temporary = populate_ast(obtain_first_kid(parseTree,termPrime, 0),NULL);
			manage_term_values(&main, &ptr1_temporary, &ptr2_temporary);
			break;
		
		case 54:	//  allVar ===> TK_RNUM
				// create Node for RNUM
			main = make_ast_node(NULL,RNUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RNUM, 1)->token);
			break;
		case 83:	// relationalOp ===> TK_GT
				// create node for Op
			main = make_ast_node(NULL,GTOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_GT, 1)->token);
			break;
		case 76: 	// var ===> TK_NUM
				// create node for NUM
			main = make_ast_node(NULL,NUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_NUM, 1)->token);
			break;
		case 71:	 // temp ===> TK_DOT TK_FIELDID
				 // ignore TK_DOT, node for FIELDID
			main = make_ast_node(NULL,FIELDID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_FIELDID, 1)->token);
			break;
		case 39:	//	new_24 ===> TK_DOT TK_FIELDID
				//	create node for FIELDID, ignore DOT
			main = make_ast_node(NULL,FIELDID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_FIELDID, 1)->token);
			break;

		case 26:	// declaration ===> TK_TYPE dataType TK_COLON TK_ID global_or_not TK_SEM
				// populate for dataType and global or not, create Node for identifier
			main = populate_ast(obtain_first_kid(parseTree, dataType, 0),parent);
			main->first_kid = make_ast_node(NULL,ID_NODE, main, NULL, NULL, obtain_first_kid(parseTree, TK_ID, 1)->token);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,global_or_not, 0),main);
			break;

		case 85:	// relationalOp ===> TK_NE
				// create node for Op
			main = make_ast_node(NULL,NEOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_NE, 1)->token);
			break;

		case 16:	// stmts ===> typeDefinitions declarations otherStmts returnStmt
				// need to create a dummy node for traversal if no type definitions
			main = populate_ast(obtain_first_kid(parseTree,typeDefinitions, 0),parent);
			if(main == NULL) {
				main = make_ast_node(NULL,TYPEDEFINITIONS_NODE, parent, NULL, NULL, obtain_first_kid(parseTree, typeDefinitions, 0)->token);
			}

			// preserve value
			ptr1_temporary = main;

			// get typedefinitions data
			main = get_last_node(main);
			main -> next = populate_ast(obtain_first_kid(parseTree, declarations, 0), parent);
			
			// empty data
			
			if(main->next == NULL){
				main->next = make_ast_node(NULL,DECLARATIONS_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,declarations, 0)->token);
			}

			// get typedefinitions node

			main = get_last_node(main); 
			//get otherStmts data
			
			main->next = populate_ast(obtain_first_kid(parseTree,otherStmts,0),parent);
			
			//empty data

			if(main->next == NULL){
				main->next = make_ast_node(NULL,OTHERSTATEMENTS_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,otherStmts, 0)->token);
			}

			// populate return stmt
			main = get_last_node(main);
			main->next = populate_ast(obtain_first_kid(parseTree,returnStmt, 0),parent);
			
			// assign to original value
			
			main = ptr1_temporary;

			break;

		case 69:	// all ===> TK_ID temp
				// create node for Identifier
				// populate for temp
			main = make_ast_node(NULL,ID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree, temp, 0),main);
			break;


		case 2:	// otherFunctions ===> function otherFunctions
				// populate for function
				// populate next for otherFunctions
			
			main = populate_ast(obtain_first_kid(parseTree,function, 0), parent);
			main->next = populate_ast(obtain_first_kid(parseTree, otherFunctions, 0),parent);
			break;

		case 67:	// all ===> TK_NUM
				// create node for NUM

			main = make_ast_node(NULL,NUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_NUM, 1)->token);
			break;


		case 37:	// singleOrRecId ===> TK_ID new_24
				// create node for TK_ID
				// populate for new_24

			main = make_ast_node(NULL,ID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,new_24, 0),main);
            	break;

		case 74:	// booleanExpression ===> TK_NOT TK_OP booleanExpression TK_CL
				// node for NOT in the center
				//         '
				//        '
				//       '
				//      booleanExpression

			main = make_ast_node(NULL,NOTOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_NOT, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,booleanExpression, 0),main);
			break;

		case 50:	// allVar ===> TK_ID var_mid
				// create node for Identifier
				// populate for var_mid

			main = make_ast_node(NULL,ID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,var_mid, 0),main);
			break;


		case 43:	// inputParameters ===> TK_SQL idList TK_SQR
				// create reference node for input parameters
				// populate for identifier list
				// ignore TK_SQL and TK_SQR

			main = make_ast_node(NULL,INPUTPARAMETERS_NODE, parent, NULL, NULL, parseTree->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,idList, 0),main);
			break;
		
		case 55:	// arithmeticExpression ===> term expPrime
				// populate for term
				// call iteratively for expPrime

			main = populate_ast(obtain_first_kid(parseTree, term, 0),parent);
			ptr1_temporary = populate_ast(obtain_first_kid(parseTree, expPrime, 0),NULL); 
			manage_term_values(&main, &ptr1_temporary, &ptr2_temporary);

            	break;
		
		case 53:	// allVar ===> TK_NUM
				// create node for TK_NUM

			main = make_ast_node(NULL,NUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_NUM, 1)->token);
			break;
		
		case 66:	// lowPrecedenceOperators ===> TK_MINUS
				// create node for MINUS Op

			main = make_ast_node(NULL,MINUSOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_MINUS, 1)->token);
			break;


		case 61:	// factor ===> TK_OP arithmeticExpression TK_CL
				// Ignore TK_OP
				// populate arithmetic expressions
				// Ignore TK_CL

			main = populate_ast(obtain_first_kid(parseTree,arithmeticExpression, 0),parent);
			break;


		case 77:	// var ===> TK_RNUM
				// create node for RNUM
		
			main = make_ast_node(NULL,RNUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RNUM, 1)->token);
			break;

		case 45:	// conditionalStmt ===> TK_IF TK_OP booleanExpression TK_CL TK_THEN stmt otherStmts elsePart
				//  create node for IF
				// ignore TK_OP
				// populate booleanExpression
				// populate stmt and otherstmts

			main = make_ast_node(NULL,IF_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_IF, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,booleanExpression, 0),main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,stmt, 0),main);
			main->first_kid->next->next = populate_ast(obtain_first_kid(parseTree, otherStmts, 0),main);
			ptr1_temporary = main->first_kid->next;
			ptr1_temporary = get_last_node(ptr1_temporary);
			ptr1_temporary->next = populate_ast(obtain_first_kid(parseTree, elsePart, 0),main);

            	break;	
		
		
		case 36:	// assignmentStmt ===> singleOrRecId TK_ASSIGNOP arithmeticExpression TK_SEM
				//                    ASSIGNOP_NODE
				//                     '
				//                   '
				//                 singleOrRecID -----------> arithmeticExpression
		
			main = make_ast_node(NULL,ASSIGNOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ASSIGNOP, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,singleOrRecId, 0),main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,arithmeticExpression, 0),main);
			break;


		case 32:	// stmt ===> iterativeStmt
				// populate on iterativeStmt

			main = populate_ast(obtain_first_kid(parseTree,iterativeStmt, 0),parent);
			break;


		case 78:	// logicalOp ===> TK_AND
				// create node for AND

		
			main = make_ast_node(NULL,ANDOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_AND, 1)->token);
			break;


		case 64:	// highPrecedenceOperators ===> TK_DIV
				// create node for TK_DIV

			main = make_ast_node(NULL,DIVOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_DIV, 1)->token);
			break;


		case 46: 	// elsePart ===> TK_ELSE stmt otherStmts TK_ENDIF
				// create node for ELSE
				// populate for stmt
				// populate for otherStmts


			main = make_ast_node(NULL,ELSE_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ELSE, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,stmt, 0),main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,otherStmts, 0),main);
			break;


		case 11: 	// primitiveDatatype ===> TK_INT
				// create node for INT

			main = make_ast_node(NULL,INT_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_INT, 1)->token);
			break;
		

		case 22:	// moreFields ===> fieldDefinition moreFields
				// populate for fieldDefinition
				// populate for moreFields

			main = populate_ast(obtain_first_kid(parseTree,fieldDefinition, 0),parent);
			main->next = populate_ast(obtain_first_kid(parseTree,moreFields, 0),parent);
			break;
		
		
		case 4: 	// function ===> TK_FUNID input_par output_par TK_SEM stmts TK_END
				// create FunctionDef node
				// popoulate for input parameters
				// populate for output parameters
				// populate for stmts
		
			main = make_ast_node(NULL,FUNCTIONDEF_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_FUNID, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,input_par, 0), main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,output_par, 0),main);
			main->first_kid->next->next = populate_ast(obtain_first_kid(parseTree,stmts, 0),main);
			break;
		

		case 68: 	// all ===> TK_RNUM
				// create node for Real Num
			
			main = make_ast_node(NULL,RNUM_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RNUM, 1)->token);
			break;


		case 65:	// lowPrecedenceOperators ===> TK_PLUS
				// create node for PLUS

			main = make_ast_node(NULL,PLUSOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_PLUS, 1)->token);
			break;


		case 5:	// input_par ===> TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
				// create node for Input Parameter
				// populate on parameter list

			main = make_ast_node(NULL,INPUTPAR_NODE, parent, NULL, NULL, obtain_first_kid(parseTree, TK_INPUT, 1)->token); // can replace obtain_first_kid(parseTree, TK_INPUT, 1)->token with NULL?
			main->first_kid = populate_ast(obtain_first_kid(parseTree,parameter_list, 0),main);
			break;
		
		
		case 42: 	// outputParameters ===> eps
				// dummy reference node if output parameters are empty
				 

			main = make_ast_node(NULL,OUTPUTPARAMETERS_NODE, parent, NULL, NULL, parseTree->token);
			main->first_kid = NULL;
			break;
		
		
		case 13: 	// constructedDatatype ===> TK_RECORD TK_RECORDID
				// create node for identifier with record label
		
		
			main = make_ast_node(NULL,RECORDID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RECORDID, 1)->token);
			break;


		case 59: 	// termPrime ===> highPrecedenceOperators factor termPrime
				// populate highPrecedence Ops
				// populate factor
				// iterate for termPrime
		
			main = populate_ast(obtain_first_kid(parseTree,highPrecedenceOperators, 0),parent);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,factor, 0),main);
			ptr1_temporary = populate_ast(obtain_first_kid(parseTree,termPrime, 0),parent);
			if(ptr1_temporary != NULL){
					manage_term_prime(&main, &ptr1_temporary, &ptr2_temporary);
					return main;
			}
            	break;



		case 49:	// ioStmt ===> TK_WRITE TK_OP allVar TK_CL TK_SEM
				// create node for IO Write
				// populate for allVar
		
			main = make_ast_node(NULL,IOWRITE_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_WRITE, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree, allVar, 0),main);
			break;


		case 81:	// relationalOp ===> TK_LE
				// create node for LE operator 

			main = make_ast_node(NULL,LEOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_LE, 1)->token);
			break;
		
		
		case 9:	// dataType ===> primitiveDatatype
				// populate for primitive data type


			main = populate_ast(obtain_first_kid(parseTree,primitiveDatatype, 0),parent);
			break;
		
		
		case 8:	// parameter_list ===> dataType TK_ID remaining_list
				// populate for data type
				// create identifier node
				// populate for remaining list
			
			main = populate_ast(obtain_first_kid(parseTree, dataType, 0),parent);
			main->first_kid = make_ast_node(NULL,ID_NODE, main, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			main->next = populate_ast(obtain_first_kid(parseTree,remaining_list, 0),parent);
			break;


		case 21:	// fieldDefinition ===> TK_TYPE primitiveDatatype TK_COLON TK_FIELDID TK_SEM
				// ignore Type
				// populate primitive data type
				// ignore colon
				// create node for field identifier

			main = populate_ast(obtain_first_kid(parseTree,primitiveDatatype, 0),parent);
			main->first_kid = make_ast_node(NULL,FIELDID_NODE, main, NULL, NULL, obtain_first_kid(parseTree,TK_FIELDID, 1)->token);
			break;
		
		
		case 6:	// output_par ===> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
				// create node to indicate output parameters
				// populate through the parameter list
			
				main =  make_ast_node(NULL,OUTPUTPAR_NODE, parent, NULL, NULL, obtain_first_kid(parseTree, TK_OUTPUT, 1)->token);
				main->first_kid = populate_ast(obtain_first_kid(parseTree,parameter_list, 0),main);
				break;

		
		case 10:	// dataType ===> constructedDatatype
				// populate constructed data type

			main = populate_ast(obtain_first_kid(parseTree,constructedDatatype, 0),parent);
			break;
		

		
		case 35:	// stmt ===> funCallStmt
				// populate function call statement

			main = populate_ast(obtain_first_kid(parseTree,funCallStmt, 0),parent);
			break;
		
		
		case 40:	// funCallStmt ===> outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM
				// create node to refer to function call
				// populate for output parameters
				// create node for function call(store lexeme)
				// populate for input parameters

			main = make_ast_node(NULL,CALL_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_CALL, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,outputParameters, 0),main);
			main->first_kid->next = make_ast_node(NULL,FUNCTIONCALL_NODE, main, NULL, NULL, obtain_first_kid(parseTree,TK_FUNID, 1)->token);
			main->first_kid->next->next = populate_ast(obtain_first_kid(parseTree,inputParameters, 0),main);
            	break;



		case 56:	// expPrime ===> lowPrecedenceOperators term expPrime
				// populate for low presedence operators
				// populate for term
				// populate for expPrime
		
			main = populate_ast(obtain_first_kid(parseTree,lowPrecedenceOperators, 0),parent);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,term, 0),main);
			ptr1_temporary = populate_ast(obtain_first_kid(parseTree,expPrime, 0),parent);
			if(ptr1_temporary != NULL){
					manage_term_prime(&main, &ptr1_temporary, &ptr2_temporary);
					return main;
			}
			break;


		case 73:	// booleanExpression ===> var relationalOp var
				// not conventional populate, relationalOp in middle for logical traversal
				//                    relationOp
				//                      '
				//                    '
				//                  var ----------------> var

			main = populate_ast(obtain_first_kid(parseTree,relationalOp, 0),parent);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,var, 0),main);
			main->first_kid->next = populate_ast(obtain_first_kid(parseTree,var, 0)->nextSibling->nextSibling,main);
			break;
		
		
		
		case 75:	// var ===> TK_ID
				// create node for ID
		
			main = make_ast_node(NULL,ID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_ID, 1)->token);
			break;
		
		
		
		case 1:	// mainFunction ===> TK_MAIN stmts TK_END
				// create node for main function
				// populate for stmts
				// ignore END

			main = make_ast_node(NULL,MAINFUNCTION_NODE, parent, NULL, NULL, obtain_first_kid(parseTree, TK_MAIN, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree,stmts, 0),main);
			break;
		
		
		case 84: 	// relationalOp ===> TK_GE
				// create node for relational Operator

			main = make_ast_node(NULL,GEOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_GE, 1)->token);
			break;
		
		
		case 82: 	// relationalOp ===> TK_EQ
				// create node for relational Operator
		
			main = make_ast_node(NULL,EQOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_EQ, 1)->token);
			break;
		
		
		case 12: 	// primitiveDatatype ===> TK_REAL
				// create node for REAL identifier


			main = make_ast_node(NULL,REAL_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_REAL, 1)->token);
			break;
		
		
		case 34: 	//  stmt ===> ioStmt
				//  populate for io statement
		
			main = populate_ast(obtain_first_kid(parseTree,ioStmt, 0),parent);
			break;
		
		
		case 90: 	// more_ids ===> TK_COMMA idList
				// ignore COMMA
				// populate for identifier list
		
			main = populate_ast(obtain_first_kid(parseTree,idList, 0),parent);
			break;
		
		
		case 27: 	// global_or_not ===> TK_COLON TK_GLOBAL
				// ignore COLON
				// create node for global identifier

			main = make_ast_node(NULL,GLOBALID_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_GLOBAL, 1)->token);
			break;
		
		
		
		case 62: 	// factor ===> all
				// populate for all

			main = populate_ast(obtain_first_kid(parseTree,all, 0),parent);
			break;
		
		
		
		case 20: 	//  fieldDefinitions ===> fieldDefinition fieldDefinition moreFields
				//  populate for field definition
				//  populate for field definition
				//  populate for more fields
		
		
			main = populate_ast(obtain_first_kid(parseTree,fieldDefinition, 0),parent);
			main->next = populate_ast(obtain_first_kid(parseTree,fieldDefinition, 0)->nextSibling,parent);
			main->next->next = populate_ast(obtain_first_kid(parseTree,moreFields, 0),parent);
			break;
		
		
		
		case 80: 	// relationalOp ===> TK_LT
				// create node for operator	 	
		
			main = make_ast_node(NULL,LTOP_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_LT, 1)->token);
			break;
		
		
		case 17: 	// typeDefinitions ===> typeDefinition typeDefinitions
				// populate for type definition
				// populate for type definitions
		
			main = populate_ast(obtain_first_kid(parseTree,typeDefinition, 0), parent);
			main->next = populate_ast(obtain_first_kid(parseTree, typeDefinitions, 0),parent);
			break;
		
		case 86: 	// returnStmt ===> TK_RETURN optionalReturn TK_SEM
				// create return node for reference
				// populate for optional return statement
				// ignore TK_SEM

			main = make_ast_node(NULL,RETURN_NODE, parent, NULL, NULL, obtain_first_kid(parseTree,TK_RETURN, 1)->token);
			main->first_kid = populate_ast(obtain_first_kid(parseTree, optionalReturn, 0),main);
			break;
		
		
		
		case 0:	// program ===> otherFunctions mainFunction
				// create node for program
				// ignore other function as child if empty for efficient memory management
				// populate on other functions
				// populate on main function
		
				main = make_ast_node(NULL,PROGRAM_NODE, parent, NULL, NULL, parseTree->token);
				main->first_kid = populate_ast(obtain_first_kid(parseTree, otherFunctions, 0), main);
				if(main->first_kid != NULL){
					astNode* lastNode = get_last_node(main->first_kid);
					lastNode->next = populate_ast(obtain_first_kid(parseTree, mainFunction, 0),main);
				}
				else{
					main->first_kid = populate_ast(obtain_first_kid(parseTree, mainFunction, 0),main);
				}
				break;
		
	}
	if(main!=NULL)
		main->parent = parent;
	return main;
}
    

astNode* createAST(parseTreeNode* tree) {
    return populate_ast(tree, NULL);
}

void print_ast(astNode* ast){
	astNode* temp = NULL ;
	if(ast != NULL) {
		temp = ast->first_kid;
		char lexeme[100];
		char parent_label[50];
		char children_label[50];
		char next_label[50];

		if (ast->token != NULL) {
			strcpy(lexeme, ast->token->lexeme);
		} else {
			strcpy(lexeme, "NULL Token");
		}
		if (ast->parent != NULL) {
			strcpy(parent_label, labelToString[ast->parent->label]);
		} else {
			strcpy(parent_label, "Root");
		}
		if (ast->first_kid != NULL) {
			strcpy(children_label, labelToString[ast->first_kid->label]);
		} else {
			strcpy(children_label, "NULL");
		}
		if (ast->next != NULL) {
			strcpy(next_label, labelToString[ast->next->label]);
		} else {
			strcpy(next_label, "NULL");
		}
		printf("%-25s %-25s %-25s %-25s %-25s\n", labelToString[ast->label], lexeme, parent_label, children_label, next_label);
	}
	while(temp != NULL){
		print_ast(temp);
		if(temp!=NULL) {
			temp = temp->next;
		}
	}
}