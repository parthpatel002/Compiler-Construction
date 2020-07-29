/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef TYPECHECKER
#define TYPECHECKER

#include "symbolTableDef.h"
extern int semantic_errors_detected;

void validate_function_semantics(astNode* root,Token* funTok);
void semantic_analysis(astNode* root);

#endif