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
#include <math.h>

#include "parser.h"
#include "parserDef.h"

int PARSER_SIZE = 0;

parseTreeNode* createNode(int isTerminal, int type, parseTreeNode* parent, Token* token, int rule_no) {
    PARSER_SIZE += 1;
    parseTreeNode* p = (parseTreeNode*) malloc(sizeof(parseTreeNode));
    p->isTerminal = isTerminal;
    if(isTerminal)
        p->symType.terminalType = (Tokentype) type;
    else 
        p->symType.nonTerminalType = (NonTerminal) type;
    p->parent = parent;
    p->first_kid = NULL;
    p->no_children = 0;
    p->token = token;
    p->nextSibling = NULL;
    p->rule_no = rule_no;

    return p;
}

parseTreeNode* initializeParseTree() {
    parseTreeNode* root = createNode(0, program, NULL, NULL, 0);
    return root;
}

parseTreeNode* addChildren(char* rule, parseTreeNode* parent) {
    char* left_NT = (char*)malloc(sizeof(char)*50);
    char* right_NT = (char*)malloc(sizeof(char)*500);
    char* useless = (char*)malloc(sizeof(char)*6);
    sscanf(rule,"%s %s %[^\n]",left_NT,useless,right_NT);
    char* word = (char*)malloc(sizeof(char)*20);
    parseTreeNode* prev, *next, *head;
    prev = NULL; next = NULL;
    do {
        word = strtok_r(right_NT, " ", &right_NT);
        if(word == NULL) {
            return head;
        }
        if(strcmp("eps", word) == 0) {
            return NULL;
        }
        if(word[0] == 'T' && word[1] == 'K') {
            next = createNode(1, pLookUp(plookupTable, word), parent, NULL, -3);
            if(prev != NULL) {
                prev->nextSibling = next;
                prev = next;
            }
            else {
                prev = next;
                head = next;
            }
        }
        else {
            next = createNode(0, pLookUp(plookupTable, word), parent, NULL, -3);
            if(prev != NULL) {
                prev->nextSibling = next;
                prev = next;
            }
            else {
                prev = next;
                head = next;
            }
        }
    }while(1);
}

void printNode(parseTreeNode* node, char** tokenMap, char** non_terminals, int isLeaf, char* empty, char* no, char* yes, char* rootMessage, FILE* outputfile){

	char* error = "(ERROR)" ;

	//Leaf Node
	if(isLeaf){	
		
		//If leaf node is a non-terminal (in-case of incomplete trees) -- ERROR CASE
		if(node->isTerminal == 0){
			
			//Root Node
			if(node->parent == NULL){
				fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,rootMessage,yes,non_terminals[node->symType.nonTerminalType]);
			}
			else{
				fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],yes,non_terminals[node->symType.nonTerminalType]);

			}
		}	

		//No lexeme for epsilon
		else if(node->symType.terminalType == EPSILON){
			fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s %s\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],yes,tokenMap[node->symType.terminalType]);
        }

		//Some terminal nodes may not be assigned lexical tokens since they are not matched -- ERROR CASE
		else if(node->token == NULL){
			fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s %s\n", error, error, error, empty, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}
		
		//Not an integer or Real Number
		else if(node->token->vtype == -1){
			fprintf(outputfile, "%-25s %-10d %-15s %-15s %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], empty, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}

		//Integer
		else if(node->token->vtype == 0){
			fprintf(outputfile, "%-25s %-10d %-15s %-15d %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], node->token->value->num, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}

		//Real
		else{
			fprintf(outputfile, "%-25s %-10d %-15s %-15f %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], node->token->value->r_num, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
        }
	
    }

	//Non Leaf Node
	else{

		//Root Node
		if(node->parent == NULL){
			fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,rootMessage,no,non_terminals[node->symType.nonTerminalType]);
		}
		else{
			fprintf(outputfile, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],no,non_terminals[node->symType.nonTerminalType]);
        }
	}
}

void printParseTreeUtility(parseTreeNode* root, char** tokenMap, char** non_terminals, FILE* outputfile) {
    
    char* empty = "----";
	char* no = "no";
	char* yes = "yes";
	char* rootMessage = "ROOT";

    if(root->first_kid != NULL) {
        printParseTreeUtility(root->first_kid, tokenMap, non_terminals, outputfile);
    }
    int isLeaf = (root->first_kid == NULL) ? 1 : 0;
    printNode(root, tokenMap, non_terminals, isLeaf,  empty, no, yes, rootMessage, outputfile);
    parseTreeNode* tmp = root->first_kid;
    if(tmp != NULL) {
        tmp = tmp -> nextSibling;
        while(tmp != NULL) {
            printParseTreeUtility(tmp, tokenMap, non_terminals, outputfile);
            tmp = tmp -> nextSibling;
        }
    }
}

void printNode2(parseTreeNode* node, char** tokenMap, char** non_terminals, int isLeaf, char* empty, char* no, char* yes, char* rootMessage){

	char* error = "(ERROR)" ;

	//Leaf Node
	if(isLeaf){	
		
		//If leaf node is a non-terminal (in-case of incomplete trees) -- ERROR CASE
		if(node->isTerminal == 0){
			
			//Root Node
			if(node->parent == NULL){
				printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,rootMessage,yes,non_terminals[node->symType.nonTerminalType]);
			}
			else{
				printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],yes,non_terminals[node->symType.nonTerminalType]);

			}
		}	

		//No lexeme for epsilon
		else if(node->symType.terminalType == EPSILON){
			printf("%-25s %-10s %-15s %-15s %-30s %-5s %s\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],yes,tokenMap[node->symType.terminalType]);
        }

		//Some terminal nodes may not be assigned lexical tokens since they are not matched -- ERROR CASE
		else if(node->token == NULL){
			printf( "%-25s %-10s %-15s %-15s %-30s %-5s %s\n", error, error, error, empty, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}
		
		//Not an integer or Real Number
		else if(node->token->vtype == -1){
			printf("%-25s %-10d %-15s %-15s %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], empty, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}

		//Integer
		else if(node->token->vtype == 0){
			printf( "%-25s %-10d %-15s %-15d %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], node->token->value->num, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
		}

		//Real
		else{
			printf( "%-25s %-10d %-15s %-15f %-30s %-5s %s\n", node->token->lexeme, node->token->line_no, tokenMap[node->token->type], node->token->value->r_num, non_terminals[node->parent->symType.nonTerminalType], yes, tokenMap[node->symType.terminalType]);
        }
	
    }

	//Non Leaf Node
	else{

		//Root Node
		if(node->parent == NULL){
			printf( "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,rootMessage,no,non_terminals[node->symType.nonTerminalType]);
		}
		else{
			printf( "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,non_terminals[node->parent->symType.nonTerminalType],no,non_terminals[node->symType.nonTerminalType]);
        }
	}
}

void printParseTreeUtility2(parseTreeNode* root, char** tokenMap, char** non_terminals) {
    
    char* empty = "----";
	char* no = "no";
	char* yes = "yes";
	char* rootMessage = "ROOT";

    if(root->first_kid != NULL) {
        printParseTreeUtility2(root->first_kid, tokenMap, non_terminals);
    }
    int isLeaf = (root->first_kid == NULL) ? 1 : 0;
    printNode2(root, tokenMap, non_terminals, isLeaf,  empty, no, yes, rootMessage);
    parseTreeNode* tmp = root->first_kid;
    if(tmp != NULL) {
        tmp = tmp -> nextSibling;
        while(tmp != NULL) {
            printParseTreeUtility2(tmp, tokenMap, non_terminals);
            tmp = tmp -> nextSibling;
        }
    }
}