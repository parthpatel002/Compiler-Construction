/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/


#ifndef SYMBOLTABLE_DEF
#define SYMBOLTABLE_DEF

#include "lexerDef.h"
#include "astDef.h"
#include "parserDef.h"

#define MAX_RECORD_LEN 30
#define REAL_WIDTH 8
#define INT_WIDTH 4
#define TABLE_SIZE 151

extern char symbol_table_errors_text[100][200];
extern int symbol_table_error_index;
typedef struct hashEntry {
	int key;
	void* node;
	struct hashEntry* next;
}hashEntry;

typedef struct hashtable{
	hashEntry* table;
	int size;
	int num_of_elements; 
} SymbolTable;



SymbolTable* initialize_symbol_table(int size);
typedef enum type{
	INT_IDENTIFIER,
	REAL_IDENTIFIER,
	RECORD_IDENTIFIER
}Type;



typedef struct field_list{
	Type type;
	int width;
	int offset;
	Token* token;
	struct field_list* next;
} field_list;




typedef struct node_r{
	Token* Tk_rec;
	field_list* fields;
	int width;
	field_list* flist;
} N_Rec;




typedef struct identifier_node{
	Token* Tk_id;
	Token* Tk_f;
	int is_global;
	N_Rec* ptr_to_rec;
	Type type;
	int width;
	int offset;
	field_list* flist;
	int is_assigned;
} N_Id;



typedef struct input_parameters{
	Token* token;
	Label label;
	struct input_parameters* next;
}input_parameters;



typedef struct output_parameters{
	Token* token;
	Label label;
	struct output_parameters* next;
}output_parameters;




typedef struct function_node{
	Token* Tk_f;
	input_parameters* par_I;
	output_parameters* par_O;
	int width;
	int cnt_of_tmp_vars;
	int order;
	field_list* flist;
}N_Func;

SymbolTable* IdentifierTable;
SymbolTable* FunctionTable;
SymbolTable* RecordTable;


typedef struct funid{
	N_Id* idNode;
	struct funid* next;
}funIDNode;

typedef struct funlist{
	Token* Tk_f;
	funIDNode* idList;
	struct funlist* next;
}funList;

#endif