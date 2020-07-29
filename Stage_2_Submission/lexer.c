/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include "lexer.h"
#include "hashTableDef.h"
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#define BUFFERSIZE 200

#define TK_ID_LEXICAL_ERROR 5
#define TK_FUNID_LEXICAL_ERROR 6
#define TK_RNUM_LEXICAL_ERROR 7
// Twin buffers
char* buffer_prev;
char* buffer_curr;

int file_over_flag = 0;
int next_ptr = 0;
int line_no = 1;

int length = 0;
char* tokenMap3[] = {"TK_ASSIGNOP",    "TK_COMMENT",    "TK_FIELDID",    "TK_ID",    "TK_NUM",    "TK_RNUM",    "TK_FUNID",    "TK_RECORDID",    "TK_WITH",    "TK_PARAMETERS",    "TK_END",    "TK_WHILE",    "TK_TYPE",    "TK_MAIN",    "TK_GLOBAL",    "TK_PARAMETER",    "TK_LIST",    "TK_SQL",    "TK_SQR",    "TK_INPUT",    "TK_OUTPUT",    "TK_INT",    "TK_REAL",    "TK_COMMA",    "TK_SEM",    "TK_COLON",    "TK_DOT",    "TK_ENDWHILE",    "TK_OP",    "TK_CL",    "TK_IF",    "TK_THEN",    "TK_ENDIF",    "TK_READ",    "TK_WRITE",    "TK_RETURN",    "TK_PLUS",    "TK_MINUS",    "TK_MUL",    "TK_DIV",    "TK_CALL",    "TK_RECORD",    "TK_ENDRECORD",    "TK_ELSE",    "TK_AND",    "TK_OR",    "TK_NOT",    "TK_LT",    "TK_LE",    "TK_EQ",    "TK_GT",    "TK_GE",    "TK_NE",    "EPSILON",   "DOLLAR", "TK_ERROR"};
char* keywords[] = {NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,  "with",    "parameters",    "end",    "while",    "type",    "_main",    "global",    "parameter",    "list",    NULL,    NULL,    "input",    "output",    "int",    "real",    NULL,    NULL,    NULL,    NULL,    "endwhile",    NULL,    NULL,    "if",    "then",    "endif",    "read",    "write",    "return",    NULL,    NULL,    NULL,    NULL,    "call",    "record",    "endrecord",    "else",    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL,   NULL,   NULL};

void appendNewLine(FILE* fp) {
    fputs("\n", fp);
    fclose(fp);
}

void addToken(Token* token, Tokentype type, char* lexeme, int vtype, Value* value){
	token->line_no = line_no;
	token->type = type;
    token->lexeme = (char*)malloc(200*sizeof(char));
	strcpy(token->lexeme, lexeme);
    Value* val = (Value*)malloc(sizeof(Value));
    if (value != NULL) {
        val->num = value->num;
        val->r_num = value->r_num;
        free(value);
    }
    token->value = val;
    token->vtype = vtype;

}

int checkForRefillBuffer(FILE** fp) {
    if(buffer_curr[next_ptr]=='\0'|| buffer_curr[next_ptr]==-1) { 
        next_ptr = 0;
        *fp = getStream(*fp);
        if(*fp == NULL) {
            file_over_flag = 1;						
        } 
	}
    return file_over_flag;
}

FILE* getStream(FILE* fp) {
    char* tmp = buffer_prev;
    buffer_prev = buffer_curr;
    buffer_curr = tmp;

    memset(buffer_curr, 0 , sizeof(char) * BUFFERSIZE + 1);

    if(feof(fp)) {
		fclose(fp);
 		return NULL;
 	}

    int noOfBytesRead;
	if((noOfBytesRead = fread(buffer_curr, sizeof(char), BUFFERSIZE, fp))>0) {
		buffer_curr[BUFFERSIZE]='\0';
 		return fp;
 	} else {
 		fclose(fp);
 		return NULL;
 	}

}

Token* getNextToken(FILE** fp) {
    
    char lex[200];

    Token* token = (Token*)malloc(sizeof(Token));

	if(file_over_flag==1) {

		if(*fp != NULL)
			fclose(*fp);

		return NULL;
	}

	//Loading Buffer
	if (checkForRefillBuffer(fp) == 1) {
        return NULL;
    }
    
	int final_state_flag = 0;
    int state = 0;
	
	int lexeme_ptr = 0;
    
    while(1) {

        if(state == 0) {
            //switch char read
            memset(lex, 0, sizeof(char) * 50);
            lexeme_ptr = 0;
        
            if(checkForRefillBuffer(fp) == 1) { // If in state 0 and file is over, then return NULL
                return NULL;
            }
            switch(buffer_curr[next_ptr]) {
                case '#' :
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 1;
                    break;
                case '_' : 
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 2;
                    length++;
                    break;
                case '=' : 
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 3;
                    break;
                case '*' : 
                    next_ptr++;
                    state = 4;
                    break;
                case '/' :  
                    next_ptr++;
                    state = 5;
                    break;
                case '+' :  
                    next_ptr++;
                    state = 6;
                    break;
                case '-' :  
                    next_ptr++;
                    state = 7;
                    break;
                case '>' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 8;
                    break;
                case '<' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 9;
                    break;
                case '~' :  
                    next_ptr++;
                    state = 10;
                    break;
                case '!' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 11;
                    break;
                case '&' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 12;
                    break;
                case '@' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 13;
                    break;
                case '[' :  
                    next_ptr++;
                    state = 14;
                    break;
                case ']' :  
                    next_ptr++;
                    state = 15;
                    break;
                case ',' :  
                    next_ptr++;
                    state = 16;
                    break;
                case ':' :  
                    next_ptr++;
                    state = 17;
                    break;
                case ';' :  
                    next_ptr++;
                    state = 18;
                    break;
                case '.' :  
                    next_ptr++;
                    state = 19;
                    break;
                case '(' :  
                    next_ptr++;
                    state = 20;
                    break;
                case ')' :  
                    next_ptr++;
                    state = 21;
                    break;
                case '%' :  
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    state = 22;
                    break;
                case ' ' :  
                case '\r' : 
                case '\t' : 
                    next_ptr++;
                    state = 0;
                    break; 
                case '\n' :
                    next_ptr++;
                    state = 0;
                    line_no++;
                    break;
                case 'a':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                    state = 23;
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    break;
                case 'b':
                case 'c':
                case 'd':
                    state = 24;
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    length++;
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    state = 25;
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    break;
                default:
                    lex[lexeme_ptr++] = buffer_curr[next_ptr];
                    next_ptr++;
                    lex[lexeme_ptr] = '\0';
                    fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                    addToken(token, TK_ERROR, lex, -2, NULL);
                    final_state_flag = 1;
            }
        }
        else {
            switch (state)
            {
                case 1:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if(buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 26;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 2:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( (buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') || (buffer_curr[next_ptr] >= 'A' && buffer_curr[next_ptr] <= 'Z')) {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        length++;
                        state = 27;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 3:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '=') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_EQ, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 4: 
                    strcpy(lex,"*");
                    addToken(token, TK_MUL, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 5:
                    strcpy(lex,"/");
                    addToken(token, TK_DIV, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 6:
                    strcpy(lex,"+");
                    addToken(token, TK_PLUS, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 7:
                    strcpy(lex,"-");
                    addToken(token, TK_MINUS, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 8:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '=') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_GE, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_GT, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 9:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '=') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_LE, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else if (buffer_curr[next_ptr] == '-') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 29;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_LT, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 10:
                    strcpy(lex,"~");
                    addToken(token, TK_NOT, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 11:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '=') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_NE, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 12:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '&') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 31;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 13:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '@') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 32;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 14:
                    strcpy(lex,"[");
                    addToken(token, TK_SQL, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 15:
                    strcpy(lex,"]");
                    addToken(token, TK_SQR, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 16:
                    strcpy(lex,",");
                    addToken(token, TK_COMMA, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 17:
                    strcpy(lex,":");
                    addToken(token, TK_COLON, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 18:
                    strcpy(lex,";");
                    addToken(token, TK_SEM, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 19: 
                    strcpy(lex,".");
                    addToken(token, TK_DOT, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 20:
                    strcpy(lex,"(");
                    addToken(token, TK_OP, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 21:
                    strcpy(lex,")");
                    addToken(token, TK_CL, lex, -1, NULL);	
                    final_state_flag = 1;
                    break;
                case 22: 
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '\n') {
                        next_ptr++;
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_COMMENT, lex, -1, NULL);
                        line_no++;
                        final_state_flag = 1;	
                    }
                    else {
                        next_ptr++;
                        state = 22;
                    }
                    break;
                case 23:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 23;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        int type;
                        if( (type = lookUp(lex) )== -1)
                            addToken(token, TK_FIELDID, lex, -1, NULL);
                        else 
                            addToken(token, type, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 24:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 23;
                    }
                    else if (buffer_curr[next_ptr] >= '2' && buffer_curr[next_ptr] <= '7') {
                        length++;
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 33;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        int type;
                        if( (type = lookUp(lex) )== -1)
                            addToken(token, TK_FIELDID, lex, -1, NULL);
                        else 
                            addToken(token, type, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 25:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= '0' && buffer_curr[next_ptr] <= '9') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 25;
                    }
                    else if (buffer_curr[next_ptr] == '.') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 35;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        int value = atoi(lex);
                        Value* val = (Value*)malloc(sizeof(Value));
                        val->num = value;

                        addToken(token, TK_NUM, lex, 0, val);
                        final_state_flag = 1;
                    }
                    break;
                case 26:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if(buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 26;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        addToken(token, TK_RECORDID, lex, -1, NULL);
                        final_state_flag = 1;
                    }

                    break;
                case 27:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if((buffer_curr[next_ptr] >= 'a' && buffer_curr[next_ptr] <= 'z') || (buffer_curr[next_ptr] >= 'A' && buffer_curr[next_ptr] <= 'Z')) {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        length++;
                        state = 27;
                    }
                    else if (buffer_curr[next_ptr] >= '0' && buffer_curr[next_ptr] <= '9') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        length++;
                        state = 28;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        int type;
                        if( (type = lookUp(lex) )== -1) {
                            if (length > 30){
                                fprintf(stderr, "Line No: %d : Function Identifier is longer than the prescribed length of 30 characters(Length = %d of %s)\n", line_no, length, lex);
                                addToken(token, TK_ERROR, lex, TK_FUNID_LEXICAL_ERROR, NULL); // for error recovery due to increased length
                                final_state_flag = 1;
                            } else {
                                addToken(token, TK_FUNID, lex, -1, NULL);
                            }
                            
                        }
                        else {
                            addToken(token, type, lex, -1, NULL);
                        }
                        final_state_flag = 1;
                    }
                    break;
                case 28:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if(buffer_curr[next_ptr] >= '0' && buffer_curr[next_ptr] <= '9') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        length++;
                        state = 28;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        int type;
                        if( (type = lookUp(lex) )== -1){
                            if (length > 30){
                                fprintf(stderr, "Line No: %d : Function Identifier is longer than the prescribed length of 30 characters(Length = %d of %s)\n", line_no, length, lex);
                                addToken(token, TK_ERROR, lex, TK_FUNID_LEXICAL_ERROR, NULL); // for error recovery due to increased length
                                final_state_flag = 1;
                            } else {
                                addToken(token, TK_FUNID, lex, -1, NULL);
                            }
                        }
                        else {
                            printf("Hereeeeeeeeeeeee\n");
                            addToken(token, type, lex, -1, NULL);
                        }
                        final_state_flag = 1;
                    }
                    break;
                case 29:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if(buffer_curr[next_ptr] == '-') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 30;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 30:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if(buffer_curr[next_ptr] == '-') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[next_ptr] = '\0';
                        addToken(token, TK_ASSIGNOP, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 31:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '&') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[next_ptr] = '\0';
                        addToken(token, TK_AND, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 32:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] == '@') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[next_ptr] = '\0';
                        addToken(token, TK_OR, lex, -1, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 33:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= 'b' && buffer_curr[next_ptr] <= 'd') {
                        length++;
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 33;
                    }
                    else if (buffer_curr[next_ptr] >= '2' && buffer_curr[next_ptr] <= '7') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        length++;
                        state = 34;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        if (length > 20){
                            fprintf(stderr, "Line No: %d : Identifier is longer than the prescribed length of 20 characters(Length = %d of %s)\n", line_no, length, lex);
                            addToken(token, TK_ERROR, lex, TK_ID_LEXICAL_ERROR, NULL); // for error recovery due to increased length
                            final_state_flag = 1;
                        } else {
                            addToken(token, TK_ID, lex, -1, NULL);
                        }
                        final_state_flag = 1;
                    }
                    break;
                case 34:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= '2' && buffer_curr[next_ptr] <= '7') {
                        length++;
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 34;
                    }
                    else if (buffer_curr[next_ptr] >= 'b' && buffer_curr[next_ptr] <= 'd') {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        if (length > 20){
                            fprintf(stderr, "Line No: %d : Identifier is longer than the prescribed length of 20 characters(Length = %d of %s)\n", line_no, length, lex);
                            addToken(token, TK_ERROR, lex, TK_ID_LEXICAL_ERROR, NULL); // for error recovery due to increased lengthca
                            final_state_flag = 1;
                        } else {
                            addToken(token, TK_ID, lex, -1, NULL);
                        }
                        final_state_flag = 1;
                    }
                    break;
                case 35:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= '0' && buffer_curr[next_ptr] <= '9') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        state = 36;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, -2, NULL);
                        final_state_flag = 1;
                    }
                    break;
                case 36:
                    if(checkForRefillBuffer(fp) == 1) {
                        return NULL;
                    }
                    if ( buffer_curr[next_ptr] >= '0' && buffer_curr[next_ptr] <= '9') {
                        lex[lexeme_ptr++] = buffer_curr[next_ptr++];
                        lex[lexeme_ptr] = '\0';
                        float value = atof(lex);
                        value = (int)(value*100) / 100.0;
                        Value* val = (Value*)malloc(sizeof(Value));
                        val->r_num = value;
                        addToken(token, TK_RNUM, lex, 1, val);
                        final_state_flag = 1;
                    }
                    else {
                        lex[lexeme_ptr] = '\0';
                        fprintf(stderr, "Line No: %d : Unknown pattern %s at line\n", line_no, lex);
                        addToken(token, TK_ERROR, lex, TK_RNUM_LEXICAL_ERROR, NULL);
                        final_state_flag = 1;
                    }
                    break;
            }
        }
        if(final_state_flag == 1) {
            length = 0;
            break;    
        }
    }

    return token;
}

void initializeLexer() {
    lookUpTable = createAndPopulateLookUpTable(keywords, TOTAL_TERMINALS_INCLUDING_ERROR, NO_SLOTS);
    buffer_curr = (char*) malloc(sizeof(char) * BUFFERSIZE + 1);
    buffer_prev = (char*) malloc(sizeof(char) * BUFFERSIZE + 1);
    memset(buffer_curr, 0, BUFFERSIZE+1);
    memset(buffer_prev, 0, BUFFERSIZE+1);
    return;
}

void unSetOverFlag() {
    file_over_flag = 0;
    line_no = 1;
}