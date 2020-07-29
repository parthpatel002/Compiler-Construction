/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef AST
#define AST

#include "lexerDef.h"
#include "astDef.h"
#include "NaryTreeDef.h"
#include "parserDef.h"


astNode* createAST(parseTreeNode* tree);
void print_ast(astNode* ast);
float return_mul(int number);
astNode* get_last_node(astNode* node);
void manage_term_values(astNode** main, astNode** ptr1_temporary, astNode** ptr2_temporary);
void manage_term_prime(astNode** main, astNode** ptr1_temporary, astNode** ptr2_temporary);
astNode* make_ast_node(astNode* temp,Label label, astNode* parent, astNode* first_kid, astNode* next, Token* token);

#endif