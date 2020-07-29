/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"

//Function Declarations

void appendNewLine(FILE* fp);
Token* getNextToken(FILE** file_ptr);
FILE* getStream(FILE* file_ptr);
void addToken(Token* token, Tokentype type, char* lexeme, int valueType, Value* value);
void initializeLexer();
void unSetOverFlag();