/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include <stdio.h>
#include <stdlib.h>

#include "parserDef.h"
#include "NaryTreeDef.h"

int hashValue(char* str, int m);
pTable* createLookUpTable(int no_of_slots);
pTable* PopulateLookUpTable(pTable* table,char** tokens,int no_of_tokens, int no_of_slots);
int pLookUp(pTable* lookupTable,char* lex);
void addToSet(int* bit_vector,int terminal);
void computeFollow(FirstAndFollow* first_follow_sets,int no_of_slots);
void computeFirst(FirstAndFollow* first_follow_sets, int no_of_slots);
FirstAndFollow* ComputeFirstAndFollowSets(int no_of_slots);
void append_sets(int* first_set,int* second_set);
int checkIfChanged(int* first_set,int* second_set);
int checkIfChangedFirst(int* first_set,int* second_set);
int computeFirstUtil(int** first, int no_of_slots);
int computeFollowUtil(int** first,int** follow, int no_of_slots);
void printFirstAndFollow(FirstAndFollow* f);
parseTable createParseTable(FirstAndFollow* firstandfollow);
int* getFirsts(char* rule_RHS, FirstAndFollow* firstandfollow);
void printParseTable(parseTable parse_table);
parseTreeNode*  parseInputSourceCode(char *testcaseFile, int** parseTable, int* errors);
void removeComments(FILE* test_case_file, FILE* cleaned_test_file);
FILE* populateGrammarDS(FILE* fp2);
void printGrammar();

////////////////////////////////////////////////////////////
parseTreeNode* createNode(int isTerminal, int type, parseTreeNode* parent, Token* token);