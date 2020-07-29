/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#ifndef HASHTABLE
#define HASHTABLE

#include "hashTableDef.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashValue(char* str, int m);
Table* createAndPopulateLookUpTable(char** tokens, int no_of_tokens, int no_of_slots);
int lookUp(char* lex);
pTable* createLookUpTable(int no_of_slots);
pTable* PopulateLookUpTable(pTable* table,char** tokens,int no_of_tokens, int no_of_slots);
int pLookUp(pTable* lookupTable,char* lex);

#endif

