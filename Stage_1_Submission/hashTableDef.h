/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef HASHTABLEDEF
#define HASHTABLEDEF

#include "lexerDef.h"

#define NUMBER_OF_SLOTS 500

struct node{
    Tokentype tType;
    char* lexeme;
    struct node* next;
};

typedef struct node Node;

typedef struct {
    struct node* head;
} Entry;



typedef struct {
    int m; // no of slots in hash table
    Entry* entries; 
} Table;

Table* lookUpTable;

struct pNode{
    int tType;
    char* lexeme;
    struct pNode* next;
};

typedef struct pNode pNode;

typedef struct {
    struct pNode* head;
} pEntry;

typedef struct {
    int m; // no of slots in hash table
    pEntry* entries; 
} pTable;

pTable* plookupTable;

#endif