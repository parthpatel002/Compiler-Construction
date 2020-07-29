/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "stack.h"
#include "NaryTree.h"
#include "hashTable.h"
#include "lexer.h"

char* tokenMap[] = {"TK_ASSIGNOP",    "TK_COMMENT",    "TK_FIELDID",    "TK_ID",    "TK_NUM",    "TK_RNUM",    "TK_FUNID",    "TK_RECORDID",    "TK_WITH",    "TK_PARAMETERS",    "TK_END",    "TK_WHILE",    "TK_TYPE",    "TK_MAIN",    "TK_GLOBAL",    "TK_PARAMETER",    "TK_LIST",    "TK_SQL",    "TK_SQR",    "TK_INPUT",    "TK_OUTPUT",    "TK_INT",    "TK_REAL",    "TK_COMMA",    "TK_SEM",    "TK_COLON",    "TK_DOT",    "TK_ENDWHILE",    "TK_OP",    "TK_CL",    "TK_IF",    "TK_THEN",    "TK_ENDIF",    "TK_READ",    "TK_WRITE",    "TK_RETURN",    "TK_PLUS",    "TK_MINUS",    "TK_MUL",    "TK_DIV",    "TK_CALL",    "TK_RECORD",    "TK_ENDRECORD",    "TK_ELSE",    "TK_AND",    "TK_OR",    "TK_NOT",    "TK_LT",    "TK_LE",    "TK_EQ",    "TK_GT",    "TK_GE",    "TK_NE",    "eps", "$", "TK_ERROR"};
char* non_terminals[] = {"primitiveDatatype", "relationalOp", "assignmentStmt","fieldDefinitions", "declaration", "moreFields", "singleOrRecId", "elsePart", "output_par", "arithmeticExpression", "mainFunction", "remaining_list", "more_ids", "returnStmt", "idList", "ioStmt", "global_or_not", "conditionalStmt", "stmts", "input_par", "allVar", "program", "booleanExpression", "highPrecedenceOperators", "inputParameters", "fieldDefinition", "iterativeStmt", "function", "all", "factor", "logicalOp", "dataType", "outputParameters", "typeDefinition", "stmt", "otherStmts", "var", "constructedDatatype", "optionalReturn", "termPrime", "term", "lowPrecedenceOperators", "temp", "parameter_list", "new_24", "declarations", "funCallStmt", "typeDefinitions", "otherFunctions", "expPrime", "var_mid"};

int no_of_rules = 92;

char* grammar[NO_OF_RULES] = {
    "program ===> otherFunctions mainFunction",
    "mainFunction ===> TK_MAIN stmts TK_END",
    "otherFunctions ===> function otherFunctions",
    "otherFunctions ===> eps",
    "function ===> TK_FUNID input_par output_par TK_SEM stmts TK_END",
    "input_par ===> TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR",
    "output_par ===> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR",
    "output_par ===> eps",
    "parameter_list ===> dataType TK_ID remaining_list",
    "dataType ===> primitiveDatatype",
    "dataType ===> constructedDatatype",
    "primitiveDatatype ===> TK_INT",
    "primitiveDatatype ===> TK_REAL",
    "constructedDatatype ===> TK_RECORD TK_RECORDID",
    "remaining_list ===> TK_COMMA parameter_list",
    "remaining_list ===> eps",
    "stmts ===> typeDefinitions declarations otherStmts returnStmt",
    "typeDefinitions ===> typeDefinition typeDefinitions",
    "typeDefinitions ===> eps",
    "typeDefinition ===> TK_RECORD TK_RECORDID fieldDefinitions TK_ENDRECORD TK_SEM",
    "fieldDefinitions ===> fieldDefinition fieldDefinition moreFields",
    "fieldDefinition ===> TK_TYPE primitiveDatatype TK_COLON TK_FIELDID TK_SEM",
    "moreFields ===> fieldDefinition moreFields",
    "moreFields ===> eps",
    "declarations ===> declaration declarations",
    "declarations ===> eps",
    "declaration ===> TK_TYPE dataType TK_COLON TK_ID global_or_not TK_SEM",
    "global_or_not ===> TK_COLON TK_GLOBAL",
    "global_or_not ===> eps",
    "otherStmts ===> stmt otherStmts",
    "otherStmts ===> eps",
    "stmt ===> assignmentStmt",
    "stmt ===> iterativeStmt",
    "stmt ===> conditionalStmt",
    "stmt ===> ioStmt",
    "stmt ===> funCallStmt",
    "assignmentStmt ===> singleOrRecId TK_ASSIGNOP arithmeticExpression TK_SEM",
    "singleOrRecId ===> TK_ID new_24",
    "new_24 ===> eps",
    "new_24 ===> TK_DOT TK_FIELDID",
    "funCallStmt ===> outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM",
    "outputParameters ===> TK_SQL idList TK_SQR TK_ASSIGNOP",
    "outputParameters ===> eps",
    "inputParameters ===> TK_SQL idList TK_SQR",
    "iterativeStmt ===> TK_WHILE TK_OP booleanExpression TK_CL stmt otherStmts TK_ENDWHILE",
    "conditionalStmt ===> TK_IF TK_OP booleanExpression TK_CL TK_THEN stmt otherStmts elsePart",
    "elsePart ===> TK_ELSE stmt otherStmts TK_ENDIF",
    "elsePart ===> TK_ENDIF",
    "ioStmt ===> TK_READ TK_OP singleOrRecId TK_CL TK_SEM",
    "ioStmt ===> TK_WRITE TK_OP allVar TK_CL TK_SEM",
    "allVar ===> TK_ID var_mid",
    "var_mid ===> TK_DOT TK_FIELDID",
    "var_mid ===> eps",
    "allVar ===> TK_NUM",
    "allVar ===> TK_RNUM",
    "arithmeticExpression ===> term expPrime",
    "expPrime ===> lowPrecedenceOperators term expPrime",
    "expPrime ===> eps",
    "term ===> factor termPrime",
    "termPrime ===> highPrecedenceOperators factor termPrime",
    "termPrime ===> eps",
    "factor ===> TK_OP arithmeticExpression TK_CL",
    "factor ===> all",
    "highPrecedenceOperators ===> TK_MUL",
    "highPrecedenceOperators ===> TK_DIV",
    "lowPrecedenceOperators ===> TK_PLUS",
    "lowPrecedenceOperators ===> TK_MINUS",
    "all ===> TK_NUM",
    "all ===> TK_RNUM",
    "all ===> TK_ID temp",
    "temp ===> eps",
    "temp ===> TK_DOT TK_FIELDID",
    "booleanExpression ===> TK_OP booleanExpression TK_CL logicalOp TK_OP booleanExpression TK_CL",
    "booleanExpression ===> var relationalOp var",
    "booleanExpression ===> TK_NOT TK_OP booleanExpression TK_CL",
    "var ===> TK_ID",
    "var ===> TK_NUM",
    "var ===> TK_RNUM",
    "logicalOp ===> TK_AND",
    "logicalOp ===> TK_OR",
    "relationalOp ===> TK_LT",
    "relationalOp ===> TK_LE",
    "relationalOp ===> TK_EQ",
    "relationalOp ===> TK_GT",
    "relationalOp ===> TK_GE",
    "relationalOp ===> TK_NE",
    "returnStmt ===> TK_RETURN optionalReturn TK_SEM",
    "optionalReturn ===> TK_SQL idList TK_SQR",
    "optionalReturn ===> eps",
    "idList ===> TK_ID more_ids",
    "more_ids ===> TK_COMMA idList",
    "more_ids ===> eps"
};

//function to make follow[i][epsilon] = 0 after computing...
void make_eps_follow_zero(FirstAndFollow* first_follow_set)
{
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        first_follow_set->follow[i][TOTAL_TERMINALS-2] = 0;
    }
}

//Adding TK_SEM in syn sets of all Non-terminals not having TK_SEM IN their first sets.
void make_tksem_as_syn()
{
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        if(parse_table[i][TK_SEM] == -1)
            parse_table[i][TK_SEM] = -2;
    }
}


void removeComments(FILE* test_case_file, FILE* cleaned_test_file) { 
    char ch;
    int comment_found = 0;
    while((ch = fgetc(test_case_file)) != EOF) {
        if (comment_found == 1 && ch == '\n') {
            fprintf(cleaned_test_file,"%c",ch);
            fprintf(stdout,"%c",ch);
            comment_found = 0;
        }
        else if (comment_found) 
            continue;
        else if (ch == '%') 
            comment_found = 1;
        else {
            fprintf(cleaned_test_file,"%c",ch);
            fprintf(stdout,"%c",ch);
        }
    }
    printf("Created cleaned file named cleaned_test_file.txt\n.");
    fclose(test_case_file);
    fclose(cleaned_test_file);
}

void addToSet(int* bit_vector,int terminal) {
    bit_vector[terminal] = 1;
}

void computeFollow(FirstAndFollow* first_follow_sets,int no_of_slots){
	int idX = pLookUp(plookupTable, "program");
	addToSet(first_follow_sets->follow[idX],TOTAL_TERMINALS-1);
	int isChanged = 1;
	while(isChanged==1){
		isChanged = 0;
		isChanged = computeFollowUtil(first_follow_sets->first,first_follow_sets->follow, no_of_slots);
	}
}

void computeFirst(FirstAndFollow* first_follow_sets, int no_of_slots) {
    int isChanged = 1;
	
    while(isChanged==1){
		isChanged = 0;
		isChanged = computeFirstUtil(first_follow_sets->first, no_of_slots);
	}

}

FirstAndFollow* ComputeFirstAndFollowSets(int no_of_slots)
{
    FirstAndFollow* first_follow_sets = (FirstAndFollow*)malloc(sizeof(FirstAndFollow));
    first_follow_sets->first = (int**)malloc(sizeof(int*)*TOTAL_NON_TERMINALS);
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        first_follow_sets->first[i] = (int*)malloc(sizeof(int)*TOTAL_TERMINALS);
        for(int j=0;j<TOTAL_TERMINALS;j++)
        {
            first_follow_sets->first[i][j] = 0;
        }
    }
    first_follow_sets->follow = (int**)malloc(sizeof(int*)*TOTAL_NON_TERMINALS);
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        first_follow_sets->follow[i] = (int*)malloc(sizeof(int)*TOTAL_TERMINALS);
        for(int j=0;j<TOTAL_TERMINALS;j++)
        {
            first_follow_sets->follow[i][j] = 0;
        }
    }
    computeFirst(first_follow_sets, plookupTable->m);
    computeFollow(first_follow_sets, plookupTable->m);
    make_eps_follow_zero(first_follow_sets);
    return first_follow_sets;
}

void append_sets(int* first_set,int* second_set)
{
    for(int i=0;i<TOTAL_TERMINALS;i++)
        if(second_set[i]==1)
            first_set[i]=1;
}


int checkIfChanged(int* first_set,int* second_set)
{
    for(int i=0;i<TOTAL_TERMINALS;i++)
        if(second_set[i] == 1 && first_set[i] == 0)
            return 1;
    return 0;
}

int checkIfChangedFirst(int* first_set,int* second_set)
{
    for(int i=0;i<TOTAL_TERMINALS - 2;i++) {
        if(second_set[i] == 1 && first_set[i] == 0)
            return 1;
    }
    return 0;
}

int computeFirstUtil(int** first, int no_of_slots) {
    int isChanged = 0;
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        for(int j=0;j<no_of_rules;j++) {
            char* left_NT = (char*)malloc(sizeof(char)*50);
            char* right_NT = (char*)malloc(sizeof(char)*500);
            char* useless = (char*)malloc(sizeof(char)*6);
            sscanf(grammar[j],"%s %s %[^\n]",left_NT,useless,right_NT);
            char* token = (char*)malloc(sizeof(char)*20);
            int idX = pLookUp(plookupTable, left_NT);
            if(idX == i) {
                do {
                    token = strtok_r(right_NT, " ", &right_NT);
                    if(token == NULL) {
                        if(first[idX][TOTAL_TERMINALS - 2] == 0) {
                            first[idX][TOTAL_TERMINALS - 2] = 1;
                            isChanged = 1;
                        }
                        break;
                    }
                    if(token[0] == 'T' && token[1] == 'K') {
                        int tidX = pLookUp(plookupTable, token);
                        if(first[idX][tidX] == 0) {
                            addToSet(first[idX], tidX);
                            isChanged = 1;
                        }
                        break;
                    }
                    else if( strcmp(token, "eps") == 0) {
                        if(first[idX][TOTAL_TERMINALS - 2] == 0) {
                            first[idX][TOTAL_TERMINALS - 2] = 1;
                            isChanged = 1;
                        }
                        break; // Adding epsilon
                    }
                    else { // non terminal
                        int tidX = pLookUp(plookupTable, token);
                        if(checkIfChangedFirst(first[idX], first[tidX]) == 1) {
                            append_sets(first[idX], first[tidX]);
                            isChanged = 1;
                        }
                        
                        if(first[tidX][TOTAL_TERMINALS - 2] == 1) { //epsilon present in tidX
                            first[idX][TOTAL_TERMINALS - 2] = 0;
                        }
                        else {
                            break;
                        }
                    }
                } while(1);
            }
        }
    }
    
    return isChanged;
}

int computeFollowUtil(int** first,int** follow, int no_of_slots)
{
    int end_flag = 0;
    int isChanged = 0;
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        for(int j=0;j<no_of_rules;j++)
        {   
            end_flag = 0;
            char* left_NT = (char*)malloc(sizeof(char)*50);
            char* right_NT = (char*)malloc(sizeof(char)*500);
            char* useless = (char*)malloc(sizeof(char)*6);
            sscanf(grammar[j],"%s %s %[^\n]",left_NT,useless,right_NT);
            char* token = (char*)malloc(sizeof(char)*20);
            while(1)
            {
                token = strtok_r(right_NT, " ", &right_NT);
                if(token==NULL)
                    break;
                if(strcmp(token, "eps") == 0) {
                    break;
                }
                if(token[0]=='T'&&token[1]=='K')
                    continue;
                else
                {
                    //non_terminal
                    int idx = pLookUp(plookupTable,token);  
                    if(idx!=i)
                        continue;
                    token = strtok_r(right_NT, " ", &right_NT);
                    if(token==NULL)
                    {
                        int tidx = pLookUp(plookupTable,left_NT);
                        
                        if(checkIfChanged(follow[idx],follow[tidx])==1)
                        {
                            append_sets(follow[idx],follow[tidx]);
                            isChanged = 1;
                        }
                    }
                    else if(token[0]=='T'&&token[1]=='K')
                    {
                        int tidx = pLookUp(plookupTable,token);
                        if(follow[idx][tidx]!=1)
                        {
                            isChanged=1;
                            addToSet(follow[idx],tidx);
                        }
                    }
                    else
                    {
                        do
                        {
                            if(token==NULL)
                            {
                                //add 
                                int tidx = pLookUp(plookupTable,left_NT);
                                if(checkIfChanged(follow[idx],follow[tidx])==1)
                                {   
                                    append_sets(follow[idx],follow[tidx]);
                                    isChanged = 1;
                                }
                                end_flag=1;
                                break;
                            }
                            //get first of the token and take union of both the sets
                            int tidx = pLookUp(plookupTable,token);
                            if(checkIfChanged(follow[idx],first[tidx])==1)
                            {
                                append_sets(follow[idx],first[tidx]);
                                isChanged = 1;
                            }
                            //append_sets(follow[idx],first[tidx]);
                            if(first[tidx][TOTAL_TERMINALS-2]!=1)
                                break;
                            token = strtok_r(right_NT, " ", &right_NT);    
                        } while(1);
                        if(end_flag)
                            break;
                    }
                } 
            }
        }
    }
    return isChanged;
}

void printFirstAndFollow(FirstAndFollow* f) {
    printf("*************************First Sets************************* \n\n\n");
    for(int i = 0; i < TOTAL_NON_TERMINALS; i++) {
        printf("%s = {", non_terminals[i]);
        for(int j = 0; j < TOTAL_TERMINALS; j++) {
            if (f->first[i][j] > 0) {
                printf("%s, ", tokenMap[j]);
            }
        }
        printf ("}\n");
    }
    printf("*************************Follow Sets************************* \n\n\n");
    for(int i = 0; i < TOTAL_NON_TERMINALS; i++) {
        printf("%s = {", non_terminals[i]);
        for(int j = 0; j < TOTAL_TERMINALS; j++) {
            if (f->follow[i][j] == 1) {
                printf("%s, ", tokenMap[j]);
            }
        }
        printf ("}\n");
    }
    return;
}

parseTable initializeParseTable(){
    parse_table = (int**)malloc(sizeof(int*)*TOTAL_NON_TERMINALS);
    for (int i=0; i<TOTAL_NON_TERMINALS; i++) {
        parse_table[i] = (int*)malloc(sizeof(int)*TOTAL_TERMINALS);
        for (int j=0; j<TOTAL_TERMINALS; j++)
            parse_table[i][j] = -1;
    }
    return parse_table;
}

int* getFirsts(char* rule_RHS, FirstAndFollow* firstandfollow) {
    char* token = (char*)malloc(sizeof(char)*50);
    int* first = (int*)malloc(sizeof(int)*TOTAL_TERMINALS);
    for (int i=0; i<TOTAL_TERMINALS; i++)
        first[i] = 0;
    token = strtok_r(rule_RHS, " ", &rule_RHS);
    int is_eps = 1;
    while (is_eps && token != NULL) {
        append_sets(first, firstandfollow->first[pLookUp(plookupTable, token)]);
        is_eps = firstandfollow -> first[pLookUp(plookupTable, token)][TOTAL_TERMINALS - 2];
        token = strtok_r(rule_RHS, " ", &rule_RHS);
    }

    if (is_eps) {
        first[TOTAL_TERMINALS - 2] = 1;
    } else {
        first[TOTAL_TERMINALS - 2] = 0;
    }
    return first;
}

parseTable createParseTable(FirstAndFollow* firstandfollow){
    parse_table = initializeParseTable();
    for (int i=0; i<no_of_rules; i++) {
        char* rule = grammar[i];
        char* left_NT = (char*)malloc(sizeof(char)*50);
        char* right_NT = (char*)malloc(sizeof(char)*500);
        char* useless = (char*)malloc(sizeof(char)*6);
        sscanf(grammar[i],"%s %s %[^\n]",left_NT,useless,right_NT);
        int lidX = pLookUp(plookupTable, left_NT);
        char* token = (char*)malloc(sizeof(char)*50);
        if ((right_NT[0] == 'T' && right_NT[1] == 'K') || (strcmp(right_NT, "eps") == 0)){
            token = strtok_r(right_NT, " ", &right_NT);
            int token_idX = pLookUp(plookupTable, token);
            if(token_idX==TOTAL_TERMINALS-2)
            {
                for(int j=0;j<TOTAL_TERMINALS;j++)
                {
                    if(firstandfollow->follow[lidX][j]==1)
                    {
                        parse_table[lidX][j] = i;
                    }
                }
                continue;
            }
            parse_table[lidX][token_idX] = i;
            continue;
        } else {
                int* firsts = getFirsts(right_NT, firstandfollow);
                if (firsts[TOTAL_TERMINALS - 2]) {
                    append_sets(firsts, firstandfollow->follow[pLookUp(plookupTable, left_NT)]);
                }
                for (int j=0; j<TOTAL_TERMINALS; j++) {
                    if (firsts[j] == 1){
                        parse_table[lidX][j] = i;
                    }
                }
        }
    }
    for(int i=0;i<TOTAL_NON_TERMINALS;i++)
    {
        if(firstandfollow->first[i][TOTAL_TERMINALS-2]==0)
        {
            for(int j=0;j<TOTAL_TERMINALS;j++)
            {
                if(firstandfollow->follow[i][j]==1 && firstandfollow->first[i][j] == 0)
                parse_table[i][j] = -2;
            }
        }
    }
    make_tksem_as_syn();
    return parse_table;
}

void printParseTable(parseTable parse_table) {
    for (int i=0; i<TOTAL_TERMINALS; i++) {
        printf("%s", tokenMap[i]);
    }
    printf("\n");
    for (int i=0; i<TOTAL_NON_TERMINALS; i++) {
        printf("%s ---- ",non_terminals[i]);
        for (int j=0; j<TOTAL_TERMINALS; j++) {
                printf("%d ", parse_table[i][j]);
        }
        printf("\n");
    }
}

parseTreeNode*  parseInputSourceCode(char *testcaseFile, int** parseTable, int* errors) {

	/**********************************************************************/

							//INITIALIZATION PHASE

	//Initialize_lexer
	// FILE* fp = initializeLexer(testcaseFile); 

    FILE *fp = fopen(testcaseFile, "r");

	if(fp == NULL){
		printf("\nFile not opened for parsing\n");
		return NULL;
	}

	printf("\n\n------------------------------LEXER INITIALIZED--------------------------\n\n");

	//Initializing ParseTree
	parseTreeNode* parse_tree = initializeParseTree();

	//Making Start symbol as the root of parse tree
	// add_root(tree);
	printf("\n\n------------------------------TREE INITIALIZED---------------------------\n\n");


	//Initializing Stack
	Stack* stack = initializeStack();

	//Pushing $ and starting nonterminal on the stack
	parseTreeNode* dollar_node =  createNode(1, DOLLAR, NULL, NULL);

	push(stack,dollar_node);
	push(stack,parse_tree);
    
	printf("\n\n------------------------------STACK INITIALIZED--------------------------\n\n");

	printf("\n\nStart obtaining tokens from lexer and parse them\n\n");

	/****************************************************************************/
								//Parsing Phase Begins

	Tokentype lookahead;

	//Initially get a token from the file
	Token* new_token = getNextToken(&fp);


	//If first token received is NULL
	if(new_token==NULL){
		printf("\n\nInput File Empty\n\n");
		return NULL;			
	}

	
	int error_found = 0;
	
	while(1){
        if (new_token == NULL) { // file is over
            StackNode* top_node = top(stack);
            if ((top_node -> stack_data -> isTerminal == 1) && (top_node -> stack_data -> symType.terminalType == DOLLAR)) {
                printf("Input source code is syntactically correct.\n");
            } else {
                *errors = 1;
                printf("Error in parsing. File over but stack not empty.\n");
            }
            break;
        }
        //received a token
        lookahead = new_token->type;

        if(lookahead == TK_ERROR) {
            *errors = 1; // Lexical error
            if(new_token->vtype == 5) {
                new_token->type = TK_ID;
                lookahead = new_token->type;
            } else if (new_token->vtype == 6) {
                new_token->type = TK_FUNID;
                lookahead = new_token->type;
            } else if (new_token->vtype == 7) {
                new_token->type = TK_RNUM;
                lookahead = new_token->type;
            } else {
                new_token = getNextToken(&fp);
                continue;
            }
        }
        else if(lookahead == TK_COMMENT){
            new_token = getNextToken(&fp);
            continue;
        }
        //not a lexical error

        //If top of the stack is $ and input is still left
		StackNode* top_node = top(stack);
        if ((top_node -> stack_data -> isTerminal == 1) && (top_node -> stack_data -> symType.terminalType == DOLLAR)) {
            *errors = 1;
            printf("\n\nUnexpected tokens at the end - Stack empty but program still left\n\n");
            printf("\n\n------Input source code is syntactically WRONG - PARSING NOT SUCCESSFUL---------------\n\n");
            break;
		}

        //normal parsing

        StackNode* snode = pop(stack);
        parseTreeNode* p = snode->stack_data;
        
        //if terminal
        if(p->isTerminal) {
            // printf ("On line %d and top of stack terminal = %d\n", new_token->line_no, snode->stack_data->symType.terminalType);
            //Symbol on the stack matches with the lookahead symbol
			if(lookahead == p->symType.terminalType) {
                // printf("Matched terminal %s at line %d\n", tokenMap[lookahead], new_token->line_no);
				//Will help in printing the parse tree
				p->token = new_token;

				//Get next Lookahead Symbol
				new_token = getNextToken(&fp);
				continue;			
			}
            else {
                // //error handling
                *errors = 1;
                // printf("####################################################Lookahead = %d\n", lookahead);
                if(p->symType.terminalType == TK_SEM) {
                    fprintf(stderr, "Line %d: Semicolon is missing\n", new_token->line_no);
                }
                else {
                    fprintf(stderr, "Line %d: The token %s for lexeme %s  does not match with the expected token %s\n", new_token->line_no, tokenMap[lookahead], new_token->lexeme, tokenMap[p->symType.terminalType]);
                }
            }
        }
        //if top of the stack is a non terminal
		else {
            // printf ("On line %d and top of stack  non terminal = %d\n", new_token->line_no, snode->stack_data->symType.nonTerminalType);
            // printf("Rule to be consulted is at %d %d\n", snode->stack_data->symType.nonTerminalType, lookahead);
            int rule_no = parseTable[snode->stack_data->symType.nonTerminalType][lookahead];
            // printf("Rule no : %d\n", rule_no);
            //No rule matches
            if(rule_no == -1) {
                //Report error
                *errors = 1;
                push(stack, p);
                new_token = getNextToken(&fp);
                
                continue;
            }
            
            else if(rule_no == -2) {
                //Handle syn
                *errors = 1;
                // printf("Inside ruleno == -2\n");
                fprintf(stderr, "Line %d: The token of type %s for lexeme %s does not match with the expected token of type %s\n", new_token->line_no, tokenMap[new_token->type], new_token->lexeme, non_terminals[p->symType.nonTerminalType]);
            }
            else {
                // printf("Here, rule no : %s___\n", grammar[rule_no]);
                //Rule matched
                //Add Children to the parse tree for the popped non terminal from the stack
                // printf("Sending rule no %s to addChildren\n", grammar[rule_no]);
				p->children = addChildren(grammar[rule_no], p);

				//PUSH RHS of the rule on the top of the stack
                if(p->children!= NULL)
				    pushAllChildrenOnStack(stack, p->children);
            }
        }
        //while loop end
    }
    if(fp!=NULL)
		fclose(fp);

	return parse_tree;
}