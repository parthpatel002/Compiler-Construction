/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/


#include <stdlib.h>
#include <stdio.h>
#include "symbolTable.h"

funList* functions = NULL;

int GLOBAL_WIDTH = 0;
int FUNCTION_ORDER = 0;

char symbol_table_errors_text[100][200];
int symbol_table_error_index = 0;


/*
    Function calculating hash using Horner's Rule.
    Required for lookup in the hash tables corresponding to Identifiers, Functions and Records.
*/
int hash(char* str, int m) {
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

/*
Create a node for record type information using the data from fields as well as token info obtained from lexical analysis.
*/
N_Rec* generateRNode(field_list* fields,Token* token){
	N_Rec* node_r = (N_Rec*)malloc(sizeof(N_Rec));
	node_r->width = 0;
	node_r->fields = fields;
	field_list* head = fields;
	node_r->Tk_rec = token;
	node_r->flist = NULL;

	while(head!=NULL){
		node_r->width += head->width;
		head = head->next;
	}
	return node_r;
}

/*
Function to generate a field list. Here, a field list is associated to a record type data structure and stores a linked list of different types of identifiers used as attributes for the same.
*/
field_list* generateFList(astNode* field_type, int* offset){
	field_list* fields = (field_list*)malloc(sizeof(field_list));
	if(field_type->label == INT_NODE){
		fields->offset = *offset;
		(*offset) += INT_WIDTH;
		fields->width = INT_WIDTH;
		fields->token = field_type->first_kid->token;
		fields->type = INT_IDENTIFIER;
	}
	else{
		fields->offset = *offset;
		(*offset) += REAL_WIDTH;
		fields->width = REAL_WIDTH;
		fields->token = field_type->first_kid->token;
		fields->type = REAL_IDENTIFIER;
	}
	return fields;
}

/*
Function used to generate an instance of output parameters. They are linked to a function instance and is required for varied tasks used in type checking and semantic analysis.
*/
output_parameters* generateOParameters(Token* token, Label label){
	output_parameters* node = (output_parameters*) malloc(sizeof(output_parameters));
	node->label = label;
	node->next = NULL;
	node->token = token;
	return node;
}

/*
Function used to generate an instance of input parameters. They are linked to a function instance and is required for varied tasks used in type checking and semantic analysis.
*/
input_parameters* generateIParameters(Token* token, Label label){
	input_parameters* node = (input_parameters*) malloc(sizeof(input_parameters));
	node->label = label;
	node->next = NULL;
	node->token = token;
	return node;
}


/*

Function used for the creation of a function instance. Has different attributes such as:

-> inputPar = a linked list for input parameters
-> outputPar = a linked list for output parameters
-> width = total width of the function corresponding to the variables declared
-> order = static scope of the function, used to handle premature function calls as a part of semantic analysis
-> temp_count = Number of temporary variables necessary for code generation

*/
N_Func* generateFNode(Token* token){
	N_Func* fnode = (N_Func*)malloc(sizeof(N_Func));
	fnode->cnt_of_tmp_vars = -1;
	fnode->order = -1;
	fnode->par_I = NULL;
	fnode->par_O = NULL;
	fnode->Tk_f = token;
	fnode->width = 0;
	fnode->flist = NULL;
	return fnode;
}


/*
Function used to generate node of type identifier. It is linked with a token corresponding to its identifier details, a function token to handle scope as well as other attributes used for type checking and semantic analysis.
*/
N_Id* generateINode(Token* Tk_id,Token* Tk_f,Token* typeToken,Type type,int is_global,int* offset) {
	N_Id* identifier = (N_Id*)malloc(sizeof(N_Id));
	N_Rec* temp_record_node = generateRNode(NULL,typeToken);
	identifier->Tk_f = Tk_f;
	identifier->type = type;
	identifier->flist = NULL;
	identifier->is_assigned = 0;
	identifier->Tk_id = Tk_id;
	if(temp_record_node->Tk_rec == NULL){
		identifier->ptr_to_rec = NULL;
	}
	else{
		identifier->ptr_to_rec = (N_Rec*)retrieve(NULL,RecordTable,temp_record_node, 2, 0);
		if(identifier->ptr_to_rec == NULL && type == RECORD_IDENTIFIER){
            fprintf(stderr, "Line no %d : No record type exists for %s at line no %d\n", 
                    typeToken->line_no, temp_record_node->Tk_rec->lexeme, typeToken->line_no);
			sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : No record type exists for %s at line no %d\n",
					typeToken->line_no, temp_record_node->Tk_rec->lexeme, typeToken->line_no);
		}
	}
	if(identifier->type == RECORD_IDENTIFIER){
		identifier->offset = *offset;
		if(identifier->ptr_to_rec!=NULL){
			identifier->width = identifier->ptr_to_rec->width;
			(*offset) += identifier->ptr_to_rec->width;
		}
	}
	else{

		identifier->offset =  *offset;
		if(identifier->type == INT_IDENTIFIER){
			identifier->width = INT_WIDTH;
			(*offset) += INT_WIDTH;
		}
		else{
			identifier->width = REAL_WIDTH;
			(*offset) += REAL_WIDTH;
		}
	} 
	identifier->is_global = is_global;
	free(temp_record_node);
	return identifier;
}


/*
Function required to remove an entry from the symbol tables. Required if an already declared variable and hence in the identifier table is declared globally in another function. This requires the removal of the initial variable to prevent faulty type checking and semantic analysis.
*/
void remove_node(SymbolTable* st, void* node){
	if(retrieve(NULL,st, node, 0, 0) == NULL){
		return;
	}
	else{
		hashEntry* previous = NULL;
		int key = hash(((N_Id*)node)->Tk_id->lexeme, st->size);
		st->num_of_elements = st->num_of_elements - 1;
		hashEntry* entry = &(st->table[key]);
		
		while(entry != NULL && !measure_i_node_similarity(entry->node,node, 0)){
			previous = entry;
			entry = entry->next;
		}
		if(entry == NULL) {
			return;
		} else {
			if(previous != NULL){
				previous->next = entry->next;
				free(entry);
			}
			else{
				if(entry->next != NULL)
					st->table[key] = *(entry->next);
				else
					st->table[key].node = NULL;
			}
		}
	}
}


/*
Function required to compare identifier nodes. Necessary when scanning the Identifier Table to check the presence of a given identifier. Used to report errors pertaining to redeclaration of identifiers.
*/
int measure_i_node_similarity(void* node1, void* node2, int symbol_table_or_type_check) {	
	N_Id* ptr2_temporary = (N_Id*)node2;
	N_Id* ptr1_temporary = (N_Id*)node1;
	if( ptr2_temporary->is_global && !(ptr1_temporary->is_global)) {
		if (symbol_table_or_type_check == 0) {
			if(strcmp(ptr1_temporary->Tk_id->lexeme, ptr2_temporary->Tk_id->lexeme)==0) {
				fprintf(stderr, "Line %d: Redeclaration of global variable %s which was declared at line %d\n",
							ptr1_temporary->Tk_id->line_no, ptr2_temporary->Tk_id->lexeme, ptr2_temporary->Tk_id->line_no);
				sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line %d: Redeclaration of global variable %s which was declared at line %d\n",
							ptr1_temporary->Tk_id->line_no, ptr2_temporary->Tk_id->lexeme, ptr2_temporary->Tk_id->line_no);
				remove_node(IdentifierTable, node1);
				return 0;
			}
		}
		return 0;
	}
	if(ptr2_temporary->is_global && ptr1_temporary->is_global ){
		if(strcmp(ptr2_temporary->Tk_id->lexeme, ptr1_temporary->Tk_id->lexeme)==0){
			return 1;
		}
	} else if(!(ptr2_temporary->is_global) && ptr1_temporary->is_global) {
		if(strcmp(ptr1_temporary->Tk_id->lexeme, ptr2_temporary->Tk_id->lexeme)==0){
			return 0;
		}
	}
	else if(ptr1_temporary->Tk_f == ptr2_temporary->Tk_f){
		if(strcmp(ptr1_temporary->Tk_id->lexeme, ptr2_temporary->Tk_id->lexeme)==0){
			return 1;
		}
	}
	return 0;
}


/*
Function required to compare function instances. Necessary when scanning the Function Table to check the presence of a given function. Used to verify information of input and output parameters of a function for type checking and semantic analysis.
*/

int measure_f_node_similarity(int similarity_quotient, void* node1, void* node2) {
	
	N_Func* ptr2_temporary = (N_Func*)node2;
    N_Func* ptr1_temporary = (N_Func*)node1;
	
	if(ptr1_temporary == ptr2_temporary) {
		similarity_quotient = 1;
	}

	if(similarity_quotient && strcmp(ptr1_temporary->Tk_f->lexeme, ptr2_temporary->Tk_f->lexeme)==0) {
		return 1;
	}
	return 0;
}


int measure_r_node_similarity(int similarity, void* node1, void* node2) {
    N_Rec* ptr1_temporary = (N_Rec*)node1;
	N_Rec* ptr2_temporary = (N_Rec*)node2;
	if(ptr2_temporary == ptr1_temporary ) {
		similarity = 1;
	}

	if(similarity && strcmp(ptr1_temporary->Tk_rec->lexeme, ptr2_temporary->Tk_rec->lexeme)==0){
		return 1;
	}
	return 0;
}


/*
Function required to insert into the various symbol tables. In place error reporting is also present if redeclaration errors are encountered. The same errors are stored in a global array for cumulative display.
*/
void insert(int to_insert, SymbolTable** st2,void* node, int node_type) {
	SymbolTable* st1 = *st2;
    int key;
	if(to_insert == 1) {
		if (node_type == 0) {
			key = hash(((N_Id*)node)->Tk_id->lexeme, st1->size);
		} else if (node_type == 1) {
			key = hash(((N_Func*)node)->Tk_f->lexeme, st1->size);
		} else {
			key = hash(((N_Rec*)node)->Tk_rec->lexeme, st1->size);
		}
		to_insert = 0;
		if(key<0)
			key += st1->size;
		if(retrieve(NULL,st1, node, node_type, 0)!=NULL){
			return;
		}
	}
	if(to_insert == 0) {
		st1 -> num_of_elements = st1 -> num_of_elements + 1;
		void* _node = st1->table[key].node;
		if (_node == NULL) {
			st1->table[key].next = NULL;
			st1->table[key].key = key;
			st1->table[key].node = node;
		}
		else{

			hashEntry* entry = &(st1->table[key]);
			while(entry->next!=NULL){
				entry = entry->next;
			}
			entry->next = (hashEntry*)malloc(sizeof(hashEntry));
			entry->next->key = key;
			entry->next->node = node;
			entry->next->next = NULL;
		}
	}
}


/*
Function used to retrieve an entry from the various symbol tables. A void pointer is returned on account of a successful retrieve which can be further typecasted as per requirement. If no entry is found, it returns a NULL.
*/
void* retrieve(field_list* f_list, SymbolTable* st1, void* node, int node_type, int symbol_table_or_type_check){
	
    if (node == NULL)
		return NULL;
    int key;
    if (f_list == NULL && node_type == 0) {
        key = hash(((N_Id*)node)->Tk_id->lexeme, st1->size);
    } else if (node_type == 1 &&f_list == NULL) {
        key = hash(((N_Func*)node)->Tk_f->lexeme, st1->size);
    } else if (node_type == 2) {
        key = hash(((N_Rec*)node)->Tk_rec->lexeme, st1->size);
    }
	void* val = st1->table[key].node;
	if(val == NULL){
		return NULL;
	} else {
		hashEntry* entry = &(st1->table[key]);
        if (node_type == 0) {
            while(entry != NULL && f_list == NULL){
                if(measure_i_node_similarity(entry->node, node, symbol_table_or_type_check)){
                    return entry -> node;
                }
                entry = entry -> next;
            }
        } else if (node_type == 1) {
            while(entry != NULL && f_list == NULL){
                if(measure_f_node_similarity(1,entry->node, node)){
                    return entry -> node;
                }
                entry = entry -> next;
            }
        } else if (node_type == 2) {
            while(entry != NULL && f_list == NULL) {
                if(measure_r_node_similarity(1,entry->node, node)){
                    return entry -> node;
                }
                entry = entry -> next;
            }
        }
		return NULL;
	}
}

/*
Initialize a symbol table for further processing.
*/
SymbolTable* initialize_symbol_table(int size) {
	SymbolTable* stPtr = (SymbolTable*)malloc(sizeof(SymbolTable));
	stPtr->num_of_elements = 0;
	stPtr->table = (hashEntry*)malloc(size*sizeof(hashEntry));
	stPtr->size = size;
	return stPtr;
}

/*
Cumulative function call to initialize every required symbol table. Uses initialize_symbol_table as a helper function.
*/
void initialize_symbol_tables(astNode* ast) {
    IdentifierTable = initialize_symbol_table(TABLE_SIZE);
	RecordTable = initialize_symbol_table(TABLE_SIZE);
	FunctionTable = initialize_symbol_table(TABLE_SIZE);
	generate_R_table(ast, NULL);
	printf("record table done\n");
	generate_F_table(ast, NULL);
	printf("function table done\n");
	generate_I_table(ast, NULL);
	printf("identifier table done\n");
}



/*
Primary function to generate the identifier table. Traverses through the abstract syntax tree and calls the hash, insert and retrieve functions along with minor semantic error reporting. Further uses the function token to handle varied scope and the corresponding variables to maintain the offset variable.
*/
void generate_I_table(astNode* root,astNode* parent) {
	int global_offset = 0;
	root = root->first_kid;
	astNode* temporary = NULL;
	while(root != NULL && parent == NULL){
		astNode* child_of_function = root->first_kid;
		N_Func* n_func = (N_Func*)malloc(sizeof(N_Func));
		n_func->Tk_f = root->token;
		n_func = ((N_Func*)retrieve(NULL,FunctionTable, (void*)n_func, 1, 0));
		int local_offset = n_func->width;
		parent = temporary;
		while(child_of_function != NULL){
			int is_global = 0;
			if(child_of_function->label == RECORDID_NODE && child_of_function->first_kid->label == ID_NODE) {
				if(child_of_function->first_kid->next != NULL){
					is_global = 1;
				}
				N_Id* identifier;
                if(is_global) {
					
                    identifier = generateINode(child_of_function->first_kid->token,
												  child_of_function->parent->token, child_of_function->token,
												  RECORD_IDENTIFIER, is_global, &global_offset);
                }
                else {
                    identifier = generateINode(child_of_function->first_kid->token,
												  child_of_function->parent->token, child_of_function->token,
												  RECORD_IDENTIFIER, is_global, &local_offset);
                }
				N_Id* temp_node = identifier;
				temporary = parent;
				if((identifier=retrieve(NULL,IdentifierTable, identifier, 0, 0)) != NULL){
					if (identifier->is_global) {
						fprintf(stderr, "Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
					} else {
						fprintf(stderr, "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
					}
					free(temp_node);
				}
				else if(identifier == NULL){
					identifier = temp_node;
					identifier->is_global = !(identifier->is_global);
					if((identifier=retrieve(NULL,IdentifierTable, identifier, 0, 0))!=NULL){ //check at test time: if condition should not be executed
						fprintf(stderr, "Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						free(temp_node);
					}	
					else {
						
						temp_node->is_global = !(temp_node->is_global);
						insert(1,&IdentifierTable, temp_node, 0);
					}

				}

			}
			else if(child_of_function->label == REAL_NODE || child_of_function->label == INT_NODE){
				if(child_of_function->first_kid->next != NULL){
					is_global = 1;
				}
				temporary = NULL;
				N_Id* identifier;
				if(is_global) {
					if(child_of_function->label == REAL_NODE) {
						identifier = generateINode(child_of_function->first_kid->token,
													child_of_function->parent->token, child_of_function->token,
													REAL_IDENTIFIER, is_global, &global_offset);
					}
					else {
						identifier = generateINode(child_of_function->first_kid->token,
													child_of_function->parent->token, child_of_function->token,
													INT_IDENTIFIER, is_global, &global_offset);
					}
                }
                else {
					if(child_of_function->label == REAL_NODE) {
						identifier = generateINode(child_of_function->first_kid->token,
													child_of_function->parent->token, child_of_function->token,
													REAL_IDENTIFIER, is_global, &local_offset);
					}
					else {
						identifier = generateINode(child_of_function->first_kid->token,
													child_of_function->parent->token, child_of_function->token,
													INT_IDENTIFIER, is_global, &local_offset);
					}
                }
				N_Id* temp_node = identifier;
				if((identifier=retrieve(NULL,IdentifierTable, identifier, 0, 0))!=NULL){
					
					if (identifier->is_global) {
						fprintf(stderr, "Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++],"Line no %d : Redeclaration of global variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
					} else {
						fprintf(stderr, "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
					}
					free(temp_node);
				}
				else if(identifier == NULL){

					identifier = temp_node;
					identifier->is_global = !(identifier->is_global);
					if((identifier=retrieve(NULL,IdentifierTable, identifier, 0, 0))!=NULL){
						
						fprintf(stderr, "Line no %d : Redeclaration of global variable %s, and previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of global variable %s, and previous declaration was at line %d\n",
							child_of_function->token->line_no, identifier->Tk_id->lexeme, identifier->Tk_id->line_no);
						free(temp_node);
					}	
					else{ 
						temp_node->is_global = !(temp_node->is_global);
						insert(1,&IdentifierTable, temp_node, 0);
					}
				}

			}
			child_of_function = child_of_function->next;
		}
		n_func->width = local_offset;
		root = root->next;
	}
	GLOBAL_WIDTH = global_offset;
}


/*
Function to generate the record table. Traverses the Abstract Syntax Tree for record type identifiers. Also simultaneously stores the fields defined under the record definition as a linked list further used for typechecking and semantic analysis.
*/
void generate_R_table(astNode* root, astNode* parent){
	root = root->first_kid;
	field_list* temp = NULL;
	field_list* fields = NULL;
	astNode* field_type = NULL;
	while(root != NULL && parent == NULL){
		astNode* child_of_function = root->first_kid;
		while(child_of_function != NULL){
			int offset = 0;
			if(child_of_function->label == RECORDID_NODE && child_of_function->first_kid->label != ID_NODE){
				field_type = child_of_function->first_kid;
				fields = generateFList(child_of_function->first_kid, &offset);
				temp = fields;
				field_type = field_type->next;
				while(field_type != NULL){
					temp->next = generateFList(field_type, &offset);
					field_type = field_type->next;
					temp = temp->next;
				}
				if(parent == NULL)
					temp->next = NULL;
				N_Rec* temp_node = NULL;
				N_Rec* record = generateRNode(fields, child_of_function->token);
				if((temp_node=retrieve(NULL,RecordTable, record, 2, 0)) != NULL) {
					sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of record %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, child_of_function->token->lexeme,
							temp_node->Tk_rec->line_no );
					fprintf(stderr, "Line no %d : Redeclaration of record %s, previous declaration was at line %d\n",
							child_of_function->token->line_no, child_of_function->token->lexeme,
							temp_node->Tk_rec->line_no);
				}
				else
					insert(1,&RecordTable, record, 2);
			}
			child_of_function = child_of_function->next;
		}
		root = root->next;
	}
}



/*
Function used to generate the function table. Traverses the Abstract Syntax Tree to search for function identifier nodes. Traverses the children and siblings to gather information and store the corresponding input and output parameters used for type checking and semantic analysis. Also based on the defined variables, allocates the width attribute of the function.
*/
void generate_F_table(astNode* root, astNode* parent){
	root = root->first_kid;
	while(root != NULL && parent == NULL){
		int local_offset = 0;
		N_Func* info_abt_function = (N_Func*)malloc(sizeof(N_Func));
		astNode* pars_of_function = NULL;
		astNode* child_of_function = root->first_kid;
		info_abt_function->order = FUNCTION_ORDER;
		FUNCTION_ORDER++;
		info_abt_function->Tk_f = root->token;
		parent = pars_of_function;
		output_parameters* current_output = NULL;
		input_parameters* current_input = NULL;
		int width = 0;
		while(child_of_function != NULL){
			if(child_of_function->label == INPUTPAR_NODE) {
				pars_of_function = child_of_function->first_kid;
				while(pars_of_function != NULL){
					N_Id* identifier;
					if(pars_of_function->token->type == TK_REAL) {
						identifier = generateINode(pars_of_function->first_kid->token, child_of_function->parent->token,
												  pars_of_function->token, REAL_IDENTIFIER,  0, &local_offset); 
					} 
					else if(pars_of_function->token->type == TK_INT) {
						identifier = generateINode(pars_of_function->first_kid->token,
												  child_of_function->parent->token,
												  pars_of_function->token,
												  INT_IDENTIFIER,
												  0,
												  &local_offset); 
					}  
					else if(pars_of_function->token->type == TK_RECORDID) {
						identifier = generateINode(pars_of_function->first_kid->token,
												  child_of_function->parent->token,
												  pars_of_function->token,
												  RECORD_IDENTIFIER,
												  0,
												  &local_offset); 
					}
					N_Id* temp_node;
					if((temp_node = (N_Id*)retrieve(NULL,IdentifierTable, identifier, 0, 0)) != NULL) {
						sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n", 
                    			child_of_function->line_no, pars_of_function->token->lexeme, temp_node->Tk_id->line_no);
						fprintf(stderr, "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n", 
                    			child_of_function->line_no, pars_of_function->token->lexeme, temp_node->Tk_id->line_no);
					}
					else{
						insert(1,&IdentifierTable, identifier, 0);
					}

					if(current_input == NULL){
						info_abt_function->par_I = generateIParameters(pars_of_function->first_kid->token, pars_of_function->label);
						current_input = generateIParameters(pars_of_function->first_kid->token, pars_of_function->label);
					}
					else{
						current_input->next = generateIParameters(pars_of_function->first_kid->token, pars_of_function->label);
						current_input = current_input->next;
					}
					width += identifier->width;
					pars_of_function = pars_of_function->next;
				}
			}
			else if(child_of_function->label == OUTPUTPAR_NODE){
				pars_of_function = child_of_function->first_kid;
				while(pars_of_function!=NULL){
					N_Id* identifier;
					if(pars_of_function->token->type == TK_REAL) {
						identifier = generateINode(pars_of_function->first_kid->token,
												  child_of_function->parent->token,
												  pars_of_function->token,
												  REAL_IDENTIFIER,
												  0,
												  &local_offset); 
					} 
					else if(pars_of_function->token->type == TK_INT) {
						identifier = generateINode(pars_of_function->first_kid->token,
												  child_of_function->parent->token,
												  pars_of_function->token,
												  INT_IDENTIFIER,
												  0,
												  &local_offset); 
					}  
					else if(pars_of_function->token->type == TK_RECORDID) {
						identifier = generateINode(pars_of_function->first_kid->token,
												  child_of_function->parent->token,
												  pars_of_function->token,
												  RECORD_IDENTIFIER,
												  0,
												  &local_offset); 
					}
					N_Id* temp_node;
					if((temp_node = (N_Id*)retrieve(NULL,IdentifierTable, identifier, 0, 0)) != NULL) {
						fprintf(stderr, "Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n", 
                    			child_of_function->line_no, pars_of_function->token->lexeme, temp_node->Tk_id->line_no);
						sprintf(symbol_table_errors_text[symbol_table_error_index++],"Line no %d : Redeclaration of variable %s, previous declaration was at line %d\n", 
                    			child_of_function->line_no, pars_of_function->token->lexeme, temp_node->Tk_id->line_no);
					}
					else{
						insert(1,&IdentifierTable, identifier, 0);
					}
					if(current_output==NULL){
						info_abt_function->par_O = generateOParameters(pars_of_function->first_kid->token, pars_of_function->label);
						current_output = info_abt_function->par_O;
					}
					else{
						current_output->next = generateOParameters(pars_of_function->first_kid->token, pars_of_function->label);
						current_output = current_output->next;
					}
					width += identifier->width;	
					pars_of_function = pars_of_function->next;
				}	
			}
			child_of_function = child_of_function->next;
		}
		info_abt_function->width = width;
		N_Func* temp_node = NULL;
		if((temp_node = (N_Func*)retrieve(NULL,FunctionTable, info_abt_function, 1, 0))!=NULL){
			fprintf(stderr, "Line no %d : Redeclaration of function %s, previous declaration at line %d\n", root->token->line_no, info_abt_function->Tk_f->lexeme, temp_node->Tk_f->line_no);
			sprintf(symbol_table_errors_text[symbol_table_error_index++], "Line no %d : Redeclaration of function %s, previous declaration at line %d\n", root->token->line_no, info_abt_function->Tk_f->lexeme, temp_node->Tk_f->line_no);
		}
		else{
			info_abt_function->cnt_of_tmp_vars = 0;
			insert(1,&FunctionTable, info_abt_function, 1);
		}
		root = root->next;
	}
}


char data_type[][30] = {
	"int",
	"real"
};


/*
Function to store the type details of a record in the form of 
"identifier_1_type x identifier_2_type...
*/
char* print_type(N_Rec* ptr_to_rec){
	field_list* fields = ptr_to_rec->fields;
	char* arr = (char*)malloc(sizeof(char)*30);
	char temp[20];
	while(fields!=NULL){
		if (fields -> next == NULL)
			sprintf(temp,"%s%s",data_type[fields->type],"");
		else
			sprintf(temp,"%s%s",data_type[fields->type], " x ");
		strcat(arr,temp);
		fields = fields->next;
	}
	return arr;
}


/*
Function to generate function identifier to be stored in function list for separating the identifier tables based on the scope.
*/
funIDNode* generate_function_id(N_Id* identifier){
	funIDNode* fid = (funIDNode*)malloc(sizeof(funIDNode));
	fid->idNode = identifier;
	fid->next = NULL;
	return fid;
}


/*
Function to generate a list of identifiers for a function required to have an individual table for each scope/function defined in the program.
*/
funList* generate_function_list(Token* Tk_f, N_Id* identifier){
	funList* function = (funList*)malloc(sizeof(funList));
	function->Tk_f = Tk_f;
	function->next = NULL;
	function->idList = generate_function_id(identifier);
	return function;
}

/*
Function required to print the complete identifier table as formed by the Abstract Syntax Tree traversal.
*/
void printSymbolTable(SymbolTable* id_table){
    int i;
    funList* function = NULL;
	funIDNode* id_list = NULL;
	hashEntry* entry;
	int size = id_table -> size;
    N_Id* identifier;
    
    functions = NULL;
    
	for(i=0; i<size; i++){
		entry = &(id_table->table[i]);
		while(entry != NULL){
			identifier = (N_Id*)(entry->node);
			if(identifier!=NULL){
				if(identifier->Tk_f == NULL){
					identifier->Tk_f = (Token*)malloc(sizeof(Token));
					strcpy(identifier->Tk_f->lexeme,"global");
				}
				if(functions != NULL){
					function = functions;
					while(function != NULL){
						if(strcmp(function->Tk_f->lexeme, identifier->Tk_f->lexeme) == 0){
							id_list = function->idList;
							while(id_list->next != NULL){
								id_list = id_list->next;
							}
							id_list->next = generate_function_id(identifier);
							break;
						}
						function = function->next;
					}
					if(function == NULL){
						function = functions;
						while(function->next!=NULL){
							function = function->next;
						}
						function->next = generate_function_list(identifier->Tk_f, identifier);
					}
				}
				else{
					functions = generate_function_list(identifier->Tk_f,identifier);
				}
			}
			entry = entry->next;
		}
	}
	char* headers[4] = {"Lexeme", "Type", "Function", "Offset"};
	printf("%-30s %-30s %-30s \t%s\n", headers[0], headers[1], headers[2], headers[3]);
	function = functions;
	while(function!=NULL){
		id_list = function->idList;
		while(id_list!= NULL){
			int is_global = id_list->idNode->is_global;
			if(!is_global){

				if (id_list -> idNode -> type < 2) {
					printf("%-30s %-30s %-30s \t%d\n", id_list->idNode->Tk_id->lexeme, data_type[id_list->idNode->type], id_list->idNode->Tk_f->lexeme, id_list->idNode->offset/2 );
				}
				else{
					printf("%-30s %-30s %-30s \t%d\n", id_list->idNode->Tk_id->lexeme, print_type(id_list->idNode->ptr_to_rec), id_list->idNode->Tk_f->lexeme, id_list->idNode->offset/2);
				}
            }
			else{
				if (id_list->idNode->type<2) {
					printf("%-30s %-30s %-30s %-30s\n", id_list->idNode->Tk_id->lexeme, data_type[id_list->idNode->type], "global","-----");
				}
				else {
					printf("%-30s %-30s %-30s %-30s\n", id_list->idNode->Tk_id->lexeme, print_type(id_list->idNode->ptr_to_rec), "global","-----");
				}			
			}
			id_list = id_list->next;
		}
		function = function->next;
	}
}


/*
Function required to print only the global identifiers as found by the Abstract Syntax Tree traversal.
*/
void printGlobalSymbolTable(SymbolTable* id_table){
     int i;
    funList* function = NULL;
	funIDNode* id_list = NULL;
	hashEntry* entry;
	int size = id_table -> size;
    N_Id* identifier;
    
    functions = NULL;

	for(i=0; i<size; i++){
		entry = &(id_table->table[i]);
		while(entry != NULL){
			identifier = (N_Id*)(entry->node);

			if(identifier!=NULL){
				if(identifier->Tk_f == NULL){
					identifier->Tk_f = (Token*)malloc(sizeof(Token));
					strcpy(identifier->Tk_f->lexeme,"global");
				}
				if(functions != NULL){
					function = functions;
					while(function!=NULL){
						if(strcmp(function->Tk_f->lexeme, identifier->Tk_f->lexeme) == 0){
							id_list = function->idList;
							while(id_list->next != NULL){
								id_list = id_list->next;
							}
							id_list->next = generate_function_id(identifier);
							break;
						}
						function = function->next;
					}
					if(function == NULL){
						function = functions;
						while(function->next!=NULL){
							function = function->next;
						}
						function->next = generate_function_list(identifier->Tk_f, identifier);
					}
				}

				else{

					functions = generate_function_list(identifier->Tk_f,identifier);
				}
			}
			entry = entry->next;
		}
	}

	function = functions;
	while(function!=NULL){
		id_list = function->idList;
		while(id_list!= NULL){
			int is_global = id_list->idNode->is_global;
			if(is_global){
					if (id_list->idNode->type<2) {
					printf("%-30s %-30s %-30s %-30s\n", id_list->idNode->Tk_id->lexeme, data_type[id_list->idNode->type], "global","-----");
				}
				else {
					printf("%-30s %-30s %-30s %-30s\n", id_list->idNode->Tk_id->lexeme, print_type(id_list->idNode->ptr_to_rec), "global","-----");
				}		
			}
			id_list = id_list->next;
		}
		function = function->next;
	}
}

/*
Function to print the Function symbol table, prints the function lexeme along with its width.
*/
void printFunctionTable(SymbolTable* fun_table){
    int i;
    hashEntry* entry;
    N_Func* fun_node;
	int size = fun_table -> size;
	for(i=0; i<size; i++){
		entry = &(fun_table->table[i]);
		while(entry != NULL){
			fun_node = (N_Func*)(entry->node);
			if(fun_node!=NULL){
                printf("%-30s %d\n",fun_node->Tk_f->lexeme,fun_node->width/2);
                printf("-------------------------------\n");
			}
			entry = entry->next;
		}
	}
}

/*
Function to print the complete set of records defined in the function along with the fields associated with record definitions.
*/
void printRecordTable(SymbolTable* rec_table){
    int i;
    hashEntry* entry;
    N_Rec* record;
	int size = rec_table -> size;
	printf("#%-30s %-30s %-30s\n", "Lexeme", "Type", "Width");
	for(i=0; i<size; i++){
		entry = &(rec_table->table[i]);
		while(entry != NULL){
			record = (N_Rec*)(entry->node);
			if(record != NULL) {
                printf("%-30s",record->Tk_rec->lexeme);
                field_list* fields = record->fields;
				char fieldsstring[100] = "";
				while (fields != NULL) {
					if(fields->width/2 == 2)
					{
						strcat(fieldsstring,"int,");
					}
					else
					{
						strcat(fieldsstring,"real,");
					}
                    fields = fields -> next;
                }
				printf("%-30s",fieldsstring);
				printf("%d\n",record->width/2);
                printf("-------------------------------\n");
			}
			entry = entry->next;
		}
	}
}


