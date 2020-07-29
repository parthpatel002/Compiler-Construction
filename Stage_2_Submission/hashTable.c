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

#include "hashTable.h"


int hashValue(char* str, int m) {
    int accumulator = 0;
    for(int i = 0; i < strlen(str); i++) {
        accumulator = accumulator * 10 + (str[i] - ' ');
        if(accumulator < 0) {
            accumulator = 30;
        }
    }
    accumulator %= m;
    return accumulator;
}

Table* createAndPopulateLookUpTable(char** tokens, int no_of_tokens, int no_of_slots) {
    Table* table = (Table*)malloc(sizeof(Table));
    table->entries = (Entry*) malloc(no_of_slots * sizeof(Entry));
    table->m = no_of_slots;
    

    for(int i = 0; i < no_of_tokens; i++) {
        if(tokens[i] == NULL) {
            continue;
        }
        Node* n = (Node*)malloc(sizeof(Node));
        n->tType = i;
        n->lexeme = (char*)malloc(sizeof(char) * 50);
        strcpy(n->lexeme, tokens[i]);
        n->next = NULL;
        int hashed_value = hashValue(tokens[i], no_of_slots);
        if(table->entries[hashed_value].head == NULL) {
            table->entries[hashed_value].head = n;
        }
        else{
            n->next = table->entries[hashed_value].head;
            table->entries[hashed_value].head = n;
        }
        // printf("%s %d %d\n  ", n->lexeme, n->tType, hashed_value);
    }
    return table;
}


int lookUp(char* lex) {
    int hashedValue = hashValue(lex, lookUpTable->m);
    Node* tmp = lookUpTable->entries[hashedValue].head;
    while(tmp != NULL) {
        if(strcmp(lex, tmp->lexeme) == 0) {
            return tmp->tType;
        }
        tmp = tmp->next;
    }
    return -1;
}

pTable* createLookUpTable(int no_of_slots)
{
    pTable* table = (pTable*)malloc(sizeof(pTable));
    table->entries = (pEntry*) malloc(no_of_slots * sizeof(pEntry));
    table->m = no_of_slots;
    return table;
}

pTable* PopulateLookUpTable(pTable* table,char** tokens,int no_of_tokens, int no_of_slots) {
    
    for(int i = 0; i < no_of_tokens; i++) {
        if(tokens[i] == NULL) {
            continue;
        }
        pNode* n = (pNode*)malloc(sizeof(pNode));
        n->tType = i;
        n->lexeme = (char*)malloc(sizeof(char) * 20);
        strcpy(n->lexeme, tokens[i]);
        n->next = NULL;
        int hashed_value = hashValue(tokens[i], no_of_slots);
        if(table->entries[hashed_value].head == NULL) {
            table->entries[hashed_value].head = n;
        }
        else{
            n->next = table->entries[hashed_value].head;
            table->entries[hashed_value].head = n;
        }
    }
    return table;
}

int pLookUp(pTable* lookupTable,char* lex) {
    int hashedValue = hashValue(lex, lookupTable->m);
    pNode* tmp = lookupTable->entries[hashedValue].head;
    while(tmp != NULL) {
        if(strcmp(lex, tmp->lexeme) == 0) {
            // printf("returned type is %s", tokenMap[tmp->tType]);
            return tmp->tType;

        }
        tmp = tmp->next;
    }
    return -1;
}