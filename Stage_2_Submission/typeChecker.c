/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include "typeChecker.h"
#include "symbolTable.h"
#include "parserDef.h"

char labelToString2[][30] = {"PROGRAM_NODE", "OTHERFUNCTIONS_NODE","MAINFUNCTION_NODE","FUNCTIONDEF_NODE","INPUTPAR_NODE","OUTPUTPAR_NODE","ID_NODE","INT_NODE","REAL_NODE","RECORDID_NODE","TYPEDEFINITIONS_NODE","DECLARATIONS_NODE","OTHERSTATEMENTS_NODE","FIELDID_NODE","GLOBALID_NODE","ASSIGNOP_NODE","CALL_NODE","FUNCTIONCALL_NODE","OUTPUTPARAMETERS_NODE","INPUTPARAMETERS_NODE","WHILE_NODE","IF_NODE","ELSE_NODE","IOREAD_NODE","IOWRITE_NODE"," NUM_NODE","RNUM_NODE","MULOP_NODE","DIVOP_NODE","PLUSOP_NODE","MINUSOP_NODE","NOTOP_NODE","LTOP_NODE","GTOP_NODE","LEOP_NODE","GEOP_NODE","EQOP_NODE","NEOP_NODE","ANDOP_NODE","OROP_NODE","RETURN_NODE"};
char labelToString3[][30] = {"INTEGER", "REAL", "RECORD"};

int semantic_errors_detected = 0;

/*
    checks assign_node and bool_node 
    checks and modifies nodes accordingly
*/
int modify_check(int* modify, astNode* assign_node, astNode* bool_node){
	if(strcmp(assign_node->token->lexeme, bool_node->token->lexeme)==0){
		return 1;
	}
	if(modify!=NULL) {
		*modify = 1;
	}
	int cmp_value=0;
	bool_node = bool_node->first_kid;
	while(bool_node!=NULL){
		cmp_value |= modify_check(modify, assign_node, bool_node);
		bool_node = bool_node->next;
	}
	return cmp_value;
}


/*
    used to validate while statements
    uses semantics of while statement
    works recursively
*/
int validate_while(astNode* bool_node_parent, astNode* statement_node_parent, astNode* bool_node, astNode* statement_node) {
	
	while(statement_node!=NULL){
		if(bool_node_parent != statement_node_parent) {
			break;
		}
		if(statement_node->label == WHILE_NODE){
			if(validate_while(NULL,NULL,bool_node, statement_node->first_kid->next)){
				return 1;
			}
		}else if(statement_node->label == ASSIGNOP_NODE){
			if(modify_check(NULL, statement_node->first_kid, bool_node)){
				return 1;
			}
		}else if(statement_node->label == IF_NODE){
			if(validate_while(NULL,NULL,bool_node, statement_node->first_kid->next)){
				return 1;
			}
		}else if(statement_node->label == ELSE_NODE){
			if(validate_while(NULL,NULL,bool_node, statement_node->first_kid)){
				return 1;
			}
		}
		statement_node = statement_node->next;
		
	}
	
	return 0;
}


/*
    used to validate boolean statements
    uses semantics of boolean statement
    works recursively
*/
N_Id* validate_boolean_expression(astNode* parent, astNode* root,Token* tk_function, int* cnt_of_tmp_vars) {
	int local_offset = 0;
	if(root->label == ID_NODE){
		astNode* field = NULL;
		astNode* b_expression = NULL;
		if(root->first_kid != NULL) {
			field = root->first_kid;
		}
		N_Id* identifier = generateINode(root->token,tk_function,NULL,INT_IDENTIFIER,0,&local_offset);
		N_Id* temp_identifier = identifier;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		parent = b_expression;
		if(identifier == NULL && parent == NULL){
			identifier = temp_identifier;
			identifier ->is_global = 1;
			identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		}
		if(identifier==NULL && parent == NULL){
			free(temp_identifier);
			fprintf(stderr, "Line no %d : In function %s undeclared variable %s.\n", root->token->line_no,
					tk_function->lexeme, root->token->lexeme);
					semantic_errors_detected++;
			return NULL;
		}
		else{
			free(temp_identifier);
			if(identifier->type == RECORD_IDENTIFIER && field != NULL){
				field_list* head = identifier->ptr_to_rec->fields;
				while (head != NULL) {
					if(strcmp(field->token->lexeme,head->token->lexeme)==0){
						return generateINode(NULL,tk_function,NULL,head->type,0,&local_offset);
					}
					head = head -> next;
				}
				
				fprintf(stderr, "Line no %d : In function %s field %s is not present in the record type %s",field->token->line_no,  tk_function->lexeme, field->token->lexeme,
						identifier->ptr_to_rec->Tk_rec->lexeme);
						semantic_errors_detected++;
				return NULL;
			}
			else if(identifier->type==RECORD_IDENTIFIER){
				fprintf(stderr, "Line no %d : In function %s record type variable %s cannot be part of boolean expression\n", root->token->line_no,
						tk_function->lexeme, root->token->lexeme);
						semantic_errors_detected++;
				return NULL;
			}
			return identifier;
		}

	}
	if(root->label == NOTOP_NODE ){
		return validate_boolean_expression(NULL,root->first_kid,tk_function,cnt_of_tmp_vars);
	}
	astNode* l_kid = root->first_kid;
	astNode* r_kid = l_kid->next;
	Type l_type=-1,r_type=-1;
	N_Id* l_node = NULL;
	N_Id* r_node = NULL;
	if (l_kid->label == NUM_NODE) {
		l_type = INT_IDENTIFIER;
	} else if (l_kid->label == RNUM_NODE) {
		l_type = REAL_IDENTIFIER;
	} else {
		l_type = -1;
	}

	if (r_kid->label == NUM_NODE) {
		r_type = INT_IDENTIFIER;
	} else if (r_kid->label == RNUM_NODE) {
		r_type = REAL_IDENTIFIER;
	} else {
		r_type = -1;
	}
	if(l_type!=-1){
		if (l_type == INT_IDENTIFIER) {
			*cnt_of_tmp_vars += INT_WIDTH;
		} else {
			*cnt_of_tmp_vars += REAL_WIDTH;
		}
	}
	if(r_type!=-1){
		if (r_type == INT_IDENTIFIER) {
			*cnt_of_tmp_vars += INT_WIDTH;
		} else {
			*cnt_of_tmp_vars += REAL_WIDTH;
		}
	}
	if(root->label == ANDOP_NODE || root->label== OROP_NODE	) {
		validate_boolean_expression(NULL,l_kid,tk_function,cnt_of_tmp_vars);
		return validate_boolean_expression(NULL,r_kid,tk_function,cnt_of_tmp_vars);
		
	}
	if(	root->label==LTOP_NODE || root->label==LEOP_NODE|| root->label==GTOP_NODE|| root->label==GEOP_NODE|| root->label==EQOP_NODE||
		root->label==NEOP_NODE ) {
	  	if(l_type==-1)
			l_node = validate_boolean_expression(NULL,l_kid,tk_function,cnt_of_tmp_vars);
		if(r_type==-1)
			r_node = validate_boolean_expression(NULL,r_kid,tk_function,cnt_of_tmp_vars);
		if(l_type==-1 && l_node!=NULL)
			l_type = l_node->type;
		if(r_type==-1 && r_node!=NULL)
			r_type = r_node->type;
		if(l_type==-1||r_type==-1)
			return NULL;
	}
	if(l_type == r_type && l_type != RECORD_IDENTIFIER){
		return generateINode(NULL,tk_function,NULL,l_type,0,&local_offset);
	}
	else{
		return NULL;
	}
}


/*
    used to validate arithematic statements
    uses semantics of arithematic statement
    works recursively

*/
N_Id* validate_arithmetic_expression(astNode* parent, astNode* root,Token* tk_function,int* cnt_of_tmp_vars){
	int local_offset = 0;
	astNode* a_expression = NULL;
	if(root->label == ID_NODE) {
		astNode* field = NULL;
		if(root->first_kid != NULL) { 
			field = root->first_kid;
		}
		N_Id* identifier = generateINode(root->token,tk_function,NULL,INT_IDENTIFIER,0,&local_offset);
		N_Id* temp_identifier = identifier;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		parent = a_expression;
		if(identifier == NULL && parent == NULL) {
			identifier = temp_identifier;
			identifier->is_global = 1;
			identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		}
		if(identifier == NULL && parent == NULL){
			free(temp_identifier);
			fprintf(stderr, "Line no %d : In function %s undeclared variable %s.\n",
                            root->token->line_no, tk_function->lexeme, root->token->lexeme);
							semantic_errors_detected++;
			return NULL;
		}
		else{
			free(temp_identifier);
			if(identifier->type != RECORD_IDENTIFIER && field != NULL){
				fprintf(stderr, "Line no %d : In function %s variable %s is not of record type.\n",
						root->token->line_no, tk_function->lexeme, root->token->lexeme);
						semantic_errors_detected++;
			}
			else if(identifier->type == RECORD_IDENTIFIER && field != NULL){
				field_list* start = identifier->ptr_to_rec->fields;
				while (start != NULL) {
					if(strcmp(field->token->lexeme,start->token->lexeme) == 0) {
						return generateINode(NULL,tk_function,NULL,start->type,0,&local_offset);
					}
					start = start -> next;
				}
				fprintf(stderr, "Line no %d : In function %s field %s is not present in the record type %s.\n",
						field->token->line_no, tk_function->lexeme, field->token->lexeme, identifier->ptr_to_rec->Tk_rec->lexeme);
				semantic_errors_detected++;
				return NULL;
			}
			return identifier;
		}

	}
	astNode* l_kid = root->first_kid;
	astNode* r_kid = l_kid->next;
	Type l_type=-1, r_type=-1;
	N_Id* l_node = NULL;
	N_Id* r_node = NULL;

	if (l_kid->label == RNUM_NODE) {
		l_type = REAL_IDENTIFIER;
	} else if (l_kid->label == NUM_NODE) {
		l_type = INT_IDENTIFIER;
	} else {
		l_type = -1;
	}

	if (r_kid->label == RNUM_NODE) {
		r_type = REAL_IDENTIFIER;
	} else if (r_kid->label == NUM_NODE) {
		r_type = INT_IDENTIFIER;
	} else {
		r_type = -1;
	}

	if( l_type != -1) {
		if (l_type == REAL_IDENTIFIER) {
			*cnt_of_tmp_vars += REAL_WIDTH;
		} else {
			*cnt_of_tmp_vars += INT_WIDTH;
		}
	}
	if(r_type != -1){
		if (r_type == REAL_IDENTIFIER) {
			*cnt_of_tmp_vars += REAL_WIDTH;
		} else {
			*cnt_of_tmp_vars += INT_WIDTH;
		}
	}
	if(	root->label == PLUSOP_NODE || root->label == MINUSOP_NODE || root->label == MULOP_NODE ||
		root->label == DIVOP_NODE || root->label == ASSIGNOP_NODE) {
	  	if(l_type ==-1)
			l_node = validate_arithmetic_expression(NULL,l_kid,tk_function,cnt_of_tmp_vars);
		if(r_type ==-1)
			r_node = validate_arithmetic_expression(NULL,r_kid,tk_function,cnt_of_tmp_vars);
		if(l_type ==-1 && l_node != NULL)
			l_type = l_node->type;
		if(r_type==-1 && r_node!=NULL)
			r_type = r_node->type;
		if(l_type==-1||r_type==-1)
			return NULL;
	}
	if(l_type == r_type && l_type != RECORD_IDENTIFIER) {
	  	if(root->label != ASSIGNOP_NODE){
	  		*cnt_of_tmp_vars = *cnt_of_tmp_vars + ((l_type == INT_IDENTIFIER)? INT_WIDTH:REAL_WIDTH);
	  	}
		return generateINode(NULL,tk_function,NULL,l_type,0,&local_offset);
	}
	else if(l_type == r_type){
		if(root->label == MULOP_NODE ||	root->label == DIVOP_NODE ){
			fprintf(stderr, "Line no %d : In function %s, can't multiply or divide two records %s, %s.\n", root->token->line_no, tk_function->lexeme,
					l_node->Tk_id->lexeme, r_node->Tk_id->lexeme);
			semantic_errors_detected++;
			return NULL;
		}
		if(l_node->ptr_to_rec != r_node->ptr_to_rec)
			return NULL;
	  	if(root->label != ASSIGNOP_NODE){
	  		*cnt_of_tmp_vars = *cnt_of_tmp_vars + l_node->ptr_to_rec->width;
	  	}
		return l_node;
	}
	else if((l_type == RECORD_IDENTIFIER && (r_type == INT_IDENTIFIER || r_type == REAL_IDENTIFIER))||
			(r_type == RECORD_IDENTIFIER && (l_type == INT_IDENTIFIER ||l_type == REAL_IDENTIFIER))) {
		if(root->label == DIVOP_NODE || root->label == MULOP_NODE) {
			if(l_type == RECORD_IDENTIFIER){
			  	if(root->label != ASSIGNOP_NODE) { 
		  			*cnt_of_tmp_vars = *cnt_of_tmp_vars + l_node->ptr_to_rec->width;
			  	}
				return l_node;
			}
			else{
				if(root->label != DIVOP_NODE){
				  	if(root->label != ASSIGNOP_NODE) {
	  					*cnt_of_tmp_vars = *cnt_of_tmp_vars + r_node->ptr_to_rec->width;
	  				}
					return r_node;
				}
				fprintf(stderr, "Line no %d : In function %s we can't divide a scalar by a record %s\n", root->token->line_no,
						 tk_function->lexeme, r_node->Tk_id->lexeme);
				semantic_errors_detected++;
				return NULL;
			}
		}
		if (l_type == RECORD_IDENTIFIER) {
			fprintf(stderr, "Line no %d : In function %s you can't add or subtract a scalar to a record %s.\n", root->token->line_no,
				tk_function->lexeme, l_node->Tk_id->lexeme);
				semantic_errors_detected++;
		} else {
			fprintf(stderr, "Line no %d : In function %s you can't add or subtract a scalar to a record %s.\n", root->token->line_no,
				tk_function->lexeme, r_node->Tk_id->lexeme);
				semantic_errors_detected++;
		}
		return NULL;
	}	
	else {
		return NULL;
	}
}


/*
      used to validate function parameters of the called function
      uses semantics of function parameters
      works recursively  

*/
int validate_function_parameters(field_list* f_list, astNode* params_output,Token* tk_function) {
	if(params_output == NULL)
		return 1;
	field_list* f_params = NULL;
	astNode* fun_identifier = params_output->next;
	astNode* params_input = fun_identifier->next->first_kid;
	N_Func* calling_function = generateFNode(tk_function);
	N_Func* temp_calling_function = calling_function;
	params_output = params_output->first_kid;
	int local_offset = 0;
	f_list = f_params;
	N_Func* called_function = generateFNode(fun_identifier->token);
	N_Func* temp_called_function = called_function;
	called_function = (N_Func*)retrieve(NULL,FunctionTable,called_function, 1, 1);
	free(temp_called_function);
	if(called_function == NULL && f_list == NULL){
		fprintf(stderr, "Line no %d : No such function %s exists at line no %d\n", 
                fun_identifier->token->line_no,fun_identifier->token->lexeme,fun_identifier->token->line_no);
				semantic_errors_detected++;
        free(temp_calling_function);
		f_params = f_list;
		return 0;
	}
	else if(tk_function == called_function->Tk_f && f_list == NULL) {
		fprintf(stderr, "Line no %d : Recursion not allowed for function %s\n", 
                fun_identifier->token->line_no, called_function->Tk_f->lexeme);
				semantic_errors_detected++;
		free(temp_calling_function);
		return 0;
	}
	else {
		calling_function = (N_Func*)retrieve(NULL,FunctionTable,calling_function, 1, 1);
		free(temp_calling_function);
		if(calling_function -> order < called_function -> order) {
			fprintf(stderr, "Line no %d : Called function %s not defined before calling function %s\n",
                    fun_identifier->token->line_no, called_function->Tk_f->lexeme, 
                    calling_function->Tk_f->lexeme);
					semantic_errors_detected++;
			return 0;
		}
	}
	output_parameters* pars_output = called_function->par_O;
	while(params_output != NULL || pars_output != NULL){
		N_Id* identifier;
		if (pars_output != NULL) {
			identifier = generateINode(pars_output->token,called_function->Tk_f,NULL,INT_IDENTIFIER,0,&local_offset);
		} else {
			identifier = NULL;
		}
		N_Id* temp_identifier = identifier;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		if(identifier == NULL){
			fprintf(stderr, "Line no  %d : In function %s extra output parameters in the function call.\n",
                    fun_identifier->token->line_no, tk_function->lexeme);
					semantic_errors_detected++;
			break;
		}
		else{
			N_Id* output_identifier;
			if (params_output != NULL) {
				output_identifier = generateINode(params_output->token,tk_function,NULL,INT_IDENTIFIER,0,&local_offset);
			} else {
				output_identifier = NULL;
			}
			N_Id* temp_output_identifier = output_identifier;
			output_identifier = (N_Id*)retrieve(NULL,IdentifierTable,output_identifier, 0, 1);
			if(output_identifier == NULL && temp_output_identifier != NULL){
				output_identifier = temp_output_identifier;
				output_identifier->is_global = 1;
				output_identifier = (N_Id*)retrieve(NULL,IdentifierTable,output_identifier, 0, 1);
			}
			if(output_identifier == NULL){
				if(params_output == NULL){
					fprintf(stderr, "Line no %d : In function %s insufficient output parameters are used.\n",
                            fun_identifier->token->line_no, tk_function->lexeme);
							semantic_errors_detected++;
				}
				else{
					fprintf(stderr, "Line no %d : In function %s undeclared variable %s.\n",
                            fun_identifier->token->line_no, tk_function->lexeme, params_output->token->lexeme);
							semantic_errors_detected++;
				}
				break;
			}
			else{
				if(identifier->type == output_identifier->type) {
					if(identifier->type == RECORD_IDENTIFIER) {
						if(identifier->ptr_to_rec != output_identifier->ptr_to_rec){
							fprintf(stderr, "Line no %d : In function %s output parameter type mismatch for %s, given type %s, required type %s for %s\n",
                                    fun_identifier->token->line_no, tk_function->lexeme, output_identifier->Tk_id->lexeme, labelToString3[output_identifier->type],
												labelToString3[identifier->type], identifier->Tk_id->lexeme);
												semantic_errors_detected++;
						}
						else
							output_identifier->is_assigned = 1;
					}
					else
						output_identifier->is_assigned = 1;
				}
				else{
					 fprintf(stderr, "Line no %d : In function %s output parameter type mismatch for %s, given type %s, required type %s for %s\n",
                                    fun_identifier->token->line_no, tk_function->lexeme, output_identifier->Tk_id->lexeme, labelToString3[output_identifier->type],
												labelToString3[identifier->type], identifier->Tk_id->lexeme);
												semantic_errors_detected++;
				}
			}
			if(temp_output_identifier != NULL)
				free(temp_output_identifier);
		}
		if(temp_identifier != NULL)
			free(temp_identifier);
		if (pars_output != NULL)
			pars_output = pars_output -> next;
		if (params_output != NULL)
			params_output = params_output -> next; 
		}

	input_parameters* pars_input = called_function->par_I;
	while(params_input != NULL || pars_input != NULL){
		input_parameters* par_I = pars_input;
		N_Id* identifier;
		if (pars_input != NULL) {
			identifier = generateINode(pars_input->token,called_function->Tk_f,NULL,INT_IDENTIFIER,0,&local_offset);
		} else {
			identifier = NULL;
		}
		N_Id* temp_identifier = identifier;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		if(identifier == NULL){
            fprintf(stderr, "Line no %d : In function %s extra input parametes used in the function call %s.\n",
                    fun_identifier->token->line_no, tk_function->lexeme, fun_identifier->token->lexeme);
					semantic_errors_detected++;
			break;
		}
		else{
			N_Id* input_identifier;
			if (params_input!=NULL) {
				input_identifier = generateINode(params_input->token,tk_function,NULL,INT_IDENTIFIER,0,&local_offset);
			} else {
				input_identifier = NULL;
			}
			N_Id* temp_input_identifier = input_identifier;
			input_identifier = (N_Id*)retrieve(NULL,IdentifierTable,input_identifier, 0, 1);
			if(input_identifier==NULL && temp_input_identifier!=NULL){
				input_identifier = temp_input_identifier;
				input_identifier->is_global = 1;
				input_identifier = (N_Id*)retrieve(NULL,IdentifierTable,input_identifier, 0, 1);
			}
			if(input_identifier==NULL){
				if(params_input==NULL){
                    fprintf(stderr, "Line no %d : In function %s insufficient input parameters.\n",
                        fun_identifier->token->line_no, tk_function->lexeme);
						semantic_errors_detected++;
				}
				else{
                    fprintf(stderr, "Line no %d : In function %s, undeclared variable %s.\n",
                        fun_identifier->token->line_no, tk_function->lexeme, params_input->token->lexeme);
						semantic_errors_detected++;
				}
				break;
			}
			else{
				if(identifier->type == input_identifier->type){
					if(identifier->type==RECORD_IDENTIFIER){
						if(identifier->ptr_to_rec != input_identifier -> ptr_to_rec){
							fprintf(stderr, "Line no %d : In function %s input parameter type mismatch for %s, given type %s, required type %s for %s\n",
                                    fun_identifier->token->line_no, tk_function->lexeme, input_identifier->Tk_id->lexeme, labelToString3[input_identifier->type],
												identifier->Tk_id->lexeme, labelToString3[identifier->type]);
												semantic_errors_detected++;
						}
					}
				}
				else{
					fprintf(stderr, "Line no %d : In function %s input parameter type mismatch for %s, given type %s, required type %s for %s\n",
                                    fun_identifier->token->line_no, tk_function->lexeme, input_identifier->Tk_id->lexeme, labelToString3[input_identifier->type],
												identifier->Tk_id->lexeme, labelToString3[identifier->type]);
												semantic_errors_detected++;
				}
			}
			if(temp_input_identifier!=NULL)
				free(temp_input_identifier);
		}
		if(temp_identifier!=NULL)
			free(temp_identifier);
		if (pars_input != NULL) {
			pars_input = pars_input -> next;
		}
		if (params_input != NULL) {
			params_input = params_input->next;
		}
	}
	return 0;	
}


/*
      used to validate read statement
      uses semantics of read statement
      works recursively

*/
N_Id* validate_read_statement(astNode* root, Token* tk_function, int* local_offset, int* error_flag) {
	N_Id* identifier = generateINode(root->first_kid->token, tk_function, NULL, INT_IDENTIFIER, 0, local_offset);
	N_Id* temp_identifier = identifier;
	identifier = (N_Id*)retrieve(NULL,IdentifierTable, identifier, 0, 1);
	*error_flag = 0;
	field_list* fields = NULL;
	if(identifier == NULL){
		identifier = temp_identifier;
		identifier->is_global = 1;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		if(identifier != NULL){
			identifier->is_assigned = 1;
		} else {
		fprintf(stderr, "Line no %d : In function %s undeclared variable %s.\n", root->token->line_no,
					tk_function->lexeme, root->first_kid->token->lexeme);
					semantic_errors_detected++;
		}
	} else{
		identifier->is_assigned = 1;
	}


	if(root->first_kid->first_kid != NULL && identifier != NULL){
		if(identifier->ptr_to_rec != NULL){
			fields = identifier->ptr_to_rec->fields;
			while (fields != NULL) {
				if(strcmp(fields->token->lexeme,root->first_kid->first_kid->token->lexeme) == 0){
					*error_flag = 1;
					break;
				}
				fields = fields -> next;
			}
			}
		else{
			fprintf(stderr, "Line no %d : In function %s variable %s is not of record type.\n",
						root->token->line_no, tk_function->lexeme, root->token->lexeme);
						semantic_errors_detected++;
			*error_flag = 1;
		}
		if(*error_flag == 0){	  			
			fprintf(stderr, "Line no %d : In function %s field %s is not present in the record type %s",fields->token->line_no,  tk_function->lexeme, fields->token->lexeme,
						identifier->ptr_to_rec->Tk_rec->lexeme);
						semantic_errors_detected++;
				return NULL;
		}
	}
	free(temp_identifier);
	return identifier;
}


/*
      used to validate write statement
      uses semantics of write statement
      works recursively
*/
N_Id* validate_write_statement(astNode* root, Token* tk_function, int* local_offset, int* error_flag) {
	N_Id* identifier = generateINode(root->first_kid->token, tk_function, NULL, INT_IDENTIFIER, 0, local_offset);
	N_Id* temp_identifier = identifier;

	identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
	
	*error_flag = 0;
	field_list* fields;
	
	if(identifier == NULL) {
		identifier = temp_identifier;
		identifier->is_global = 1;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
		if(identifier == NULL) {
			fprintf(stderr, "Line no %d : In function %s undeclared variable %s.\n", root->token->line_no,
					tk_function->lexeme, root->first_kid->token->lexeme);
					semantic_errors_detected++;
		}
	}
	if(root->first_kid->first_kid != NULL && identifier != NULL){
		if(identifier->ptr_to_rec != NULL){
			fields = identifier->ptr_to_rec->fields;
			while (fields != NULL) {
				if(strcmp(fields->token->lexeme, root->first_kid->first_kid->token->lexeme)==0){
					*error_flag = 1;
					break;
				}
				fields = fields -> next;
			}
		}
		else{

			fprintf(stderr, "Line no %d : In function %s variable %s is not of record type.\n",
						root->token->line_no, tk_function->lexeme, root->token->lexeme);
						semantic_errors_detected++;
			*error_flag = 1;
		}
		if(*error_flag == 0) {
			fprintf(stderr, "Line no %d : In function %s field %s is not present in the record type %s",fields->token->line_no,  tk_function->lexeme, fields->token->lexeme,
						identifier->ptr_to_rec->Tk_rec->lexeme);
						semantic_errors_detected++;
			return NULL;
		}
	}
	free(temp_identifier);
	return identifier;
}


/*
      used to validate return statement
      uses semantics of return statement
      works recursively
*/
void validate_return_expression (astNode* root_node, Token* tk_function){
	N_Func* n_func = (N_Func*)malloc(sizeof(N_Func));
	n_func->Tk_f = tk_function;
	n_func = ((N_Func*)retrieve(NULL,FunctionTable, n_func, 1, 1));
	astNode* child_of_function = root_node;
	astNode* return_fun = root_node->first_kid;
	int local_offset = 0;
	N_Id* temp_identifier = NULL;
	output_parameters* pars_output = n_func->par_O;
	while(return_fun != NULL || pars_output != NULL){
		N_Id* identifier;
		if (pars_output == NULL) {
			identifier = NULL;
		} else {
			identifier = generateINode(pars_output->token, n_func->Tk_f, NULL, INT_IDENTIFIER, 0, &local_offset);
		}

		temp_identifier = identifier;
		identifier = (N_Id*)retrieve(NULL,IdentifierTable, identifier, 0, 1);
		if(identifier != NULL) {
			N_Id* return_identifier;
			if (return_fun == NULL) {
				return_identifier = NULL;
			} else {
				return_identifier = generateINode(return_fun->token,n_func->Tk_f, NULL, INT_IDENTIFIER, 0, &local_offset);
				
			}
			N_Id* temp_return_identifier = return_identifier;
			return_identifier = (N_Id*)retrieve(NULL,IdentifierTable, return_identifier, 0, 1);
			if(return_identifier == NULL && temp_return_identifier != NULL){
				return_identifier = temp_return_identifier;
				return_identifier->is_global = 1;
				return_identifier = (N_Id*)retrieve(NULL,IdentifierTable, return_identifier, 0, 1);
			}
			if(return_identifier == NULL){
				if (temp_return_identifier == NULL) {
					fprintf(stderr, "Line no %d : Insufficient return parameters in function %s\n", 
							child_of_function->token->line_no, n_func->Tk_f->lexeme);
							semantic_errors_detected++;
				} else {
					fprintf(stderr, "Line no %d : Insufficient return parameters in function %s probably due to variable %s not being defined\n", 
							child_of_function->token->line_no, n_func->Tk_f->lexeme, temp_return_identifier->Tk_id->lexeme);
							semantic_errors_detected++;
					}
				break;
			}
			else{
				if(identifier->type != return_identifier->type){
					fprintf(stderr, "Line no %d : Return type mismatch in function %s for %s parameter given type %s, required type %s for %s\n",
                                    child_of_function->token->line_no, n_func->Tk_f->lexeme, return_identifier->Tk_id->lexeme, labelToString3[return_identifier->type], labelToString3[identifier->type],
												identifier->Tk_id->lexeme);
												semantic_errors_detected++;
				}
				else{
					if(identifier -> type == RECORD_IDENTIFIER){
						if(identifier->ptr_to_rec != return_identifier->ptr_to_rec){
							fprintf(stderr, "Line no %d : Return type mismatch in function %s for %s parameter given type %s, required type %s for %s\n",
                                    child_of_function->token->line_no, n_func->Tk_f->lexeme, return_identifier->Tk_id->lexeme, labelToString3[return_identifier->type], labelToString3[identifier->type],
												identifier->Tk_id->lexeme);
												semantic_errors_detected++;
							}
						else if(identifier != return_identifier){
								fprintf(stderr, "Line no %d : Input parameter %s does not match output parameter %s in function %s\n", 
									child_of_function->token->line_no, identifier->Tk_id->lexeme, return_identifier->Tk_id->lexeme, n_func->Tk_f->lexeme);
									semantic_errors_detected++;
						}
						else if(return_identifier->is_assigned == 0){
								fprintf(stderr, "Line no %d : Return parameter %s is not assigned in function %s\n", 
									child_of_function->token->line_no, return_identifier->Tk_id->lexeme, n_func->Tk_f->lexeme);
									semantic_errors_detected++;
						}
					}
					else{
						if(identifier != return_identifier){
								fprintf(stderr, "Line no %d : Input parameter %s does not match output parameter %s in function %s\n", 
									child_of_function->token->line_no, identifier->Tk_id->lexeme, return_identifier->Tk_id->lexeme, n_func->Tk_f->lexeme);
									semantic_errors_detected++;
						}
						else if(identifier->is_assigned == 0){
								fprintf(stderr, "Line no %d : Return parameter %s is not assigned in function %s\n", 
									child_of_function->token->line_no, return_identifier->Tk_id->lexeme, n_func->Tk_f->lexeme);
									semantic_errors_detected++;
						} 
					}
				}
			}
			if(temp_return_identifier != NULL)
				free(temp_return_identifier);
		}
		else{
			fprintf(stderr, "Line no %d : Extra return parameters encountered in function %s\n", child_of_function->token->line_no, tk_function->lexeme);
			semantic_errors_detected++;
			break;
		}
		if(temp_identifier != NULL)
			free(temp_identifier);
		if (pars_output != NULL) 
			pars_output = pars_output -> next;
		if (return_fun != NULL)
			return_fun = return_fun -> next;
	}
}


void validate_function_semantics(astNode* root_node,Token* tk_function) {
	if(root_node == NULL){
		return;
	}
	astNode* child_of_function = root_node->first_kid;
	N_Func* info_abt_function = (N_Func*)malloc(sizeof(N_Func));
	N_Func* temp_fun_info = info_abt_function;
	info_abt_function->Tk_f = tk_function;
	info_abt_function = (N_Func*)retrieve(NULL,FunctionTable,info_abt_function, 1, 1);
	free(temp_fun_info);
	N_Id* identifier = NULL;
	N_Id* temp_node = NULL;
	field_list* field = NULL;
	int field_flag = 0; 
	while(child_of_function != NULL) { 
		int cnt_of_tmp_vars = 0;
		int local_offset = 0;
        Label label = child_of_function->label;
		switch(label) {
			case CALL_NODE : validate_function_parameters(NULL,child_of_function->first_kid,tk_function);
						  break;
			case IF_NODE: validate_boolean_expression(NULL,child_of_function->first_kid,tk_function,&cnt_of_tmp_vars);
						if(info_abt_function->cnt_of_tmp_vars < cnt_of_tmp_vars){
							info_abt_function->cnt_of_tmp_vars = cnt_of_tmp_vars;
						}
						break;
			case RETURN_NODE: validate_return_expression(child_of_function, tk_function);
							break;
		    case IOREAD_NODE:
							identifier = validate_read_statement(child_of_function, tk_function, &local_offset, &field_flag);
							  break;
		    case IOWRITE_NODE:
							identifier = validate_write_statement(child_of_function, tk_function, &local_offset, &field_flag);
							  break;
			case ASSIGNOP_NODE : validate_arithmetic_expression(NULL,child_of_function,tk_function, &cnt_of_tmp_vars);
							  identifier = generateINode(child_of_function->first_kid->token,tk_function,NULL,INT_IDENTIFIER,0,&local_offset);
							  temp_node = identifier;
							  identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
							  if(identifier!=NULL){
							  		identifier->is_assigned = 1;
							  }
							  else{
							  	identifier = temp_node;
							  	identifier->is_global = 1;
							  	identifier = (N_Id*)retrieve(NULL,IdentifierTable,identifier, 0, 1);
							  	if(identifier != NULL)
							  		identifier->is_assigned = 1;
							  }
							  free(temp_node);
							  if(info_abt_function->cnt_of_tmp_vars < cnt_of_tmp_vars){
							  	info_abt_function ->cnt_of_tmp_vars = cnt_of_tmp_vars;
							  }
							break;
			case WHILE_NODE: validate_boolean_expression(NULL,child_of_function->first_kid,tk_function,&cnt_of_tmp_vars);
							if(!validate_while(NULL,NULL,child_of_function->first_kid,child_of_function->first_kid->next)){
								fprintf(stderr, "Line no %d : In function %s while loop probably would not terminate\n", child_of_function->token->line_no, tk_function->lexeme);
								semantic_errors_detected++;
							}
						  break;
		}
		validate_function_semantics(child_of_function,tk_function);
		child_of_function = child_of_function->next;
	}
}


/*
    calls all validate expressions for type checking
    main function to be invoked for type checking
*/
void semantic_analysis(astNode* root_node){
	root_node = root_node->first_kid;
	while(root_node != NULL){
		validate_function_semantics(root_node,root_node->token);
		root_node = root_node->next;
	}
}