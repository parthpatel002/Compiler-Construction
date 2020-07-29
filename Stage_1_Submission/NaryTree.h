/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef NARYTREE
#define NARYTREE

#include "NaryTreeDef.h"

parseTreeNode* createNode(int isTerminal, int type, parseTreeNode* parent, Token* token);
parseTreeNode* initializeParseTree();
parseTreeNode* addChildren(char* rule, parseTreeNode* parent);
void printParseTreeUtility(parseTreeNode* root, char** tokenMap, char** nonterminals, FILE* outputfile);
void printNode(parseTreeNode* root, char** tokenMap, char** non_terminals, FILE* outputfile);

#endif