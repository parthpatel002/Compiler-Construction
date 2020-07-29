/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/


#ifndef STACKDEF
#define STACKDEF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserDef.h"
#include "NaryTreeDef.h"

typedef parseTreeNode stackData;
struct StackNode{
	stackData* stack_data;
	struct StackNode* next;
};

typedef struct StackNode StackNode;

typedef struct {
	StackNode* bottom;
	StackNode* top;
	int size;
} Stack;

#endif