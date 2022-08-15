#include <generation/arch/x86-64/generator.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <util/util.h>
#include <util/logging/logger.h>
#include <conventions/lcc.h>

#include <parsing/scope_impl.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/literal.h>
#include <parsing/nodes/enum.h>

#include <types/datatype.h>
#include <types/table.h>

#include <generation/arch/x86-64/assembly/section.h>
#include <generation/arch/x86-64/assembly/program.h>
#include <generation/arch/x86-64/assembly/registers.h>
#include <generation/arch/x86-64/assembly/stack.h>

// ------------------------------------------------------------------------

#define NUM_ARGS(...) (int)(sizeof((char*[]){__VA_ARGS__})/sizeof(char*))
#define STRINGIFY(...) #__VA_ARGS__

#define DEFINE_GLOBAL(name) \
	arraylist_add(assembly->global_directives, strdup((char*)name))

#define DEFINE_EXTERN(name) \
	arraylist_add(assembly->extern_directives, strdup((char*)name))

// labels
#define ADD_TEXT_LABEL(label) \
	text_section_add_label(assembly->text, label)

// instructions

#define ADD_INST(mnemonic, ...) \
	text_section_add_instruction(assembly->text, mnemonic, (char *[]){__VA_ARGS__}, NUM_ARGS(__VA_ARGS__))

// variables in data section

#define DECLARE_VARIABLE(label, type, value) \
	data_section_declare_variable(assembly->data, label, type, value)

// variable in bss section

#define ADD_UNINITIALIZED_VAR(label, type, count) \
	bss_section_declare_space(assembly->bss, label, type, count)

// ------------------------------------------------------------------------

void add__start_function(Function *main_func);

bool evaluate_package(Package *package);

bool evaluate_global_variables(ArrayList *global_variables);
bool evaluate_global_variable(Variable *variable);
bool generate_extern_functions(ArrayList *extern_functions);
bool generate_imported_functions(ArrayList *imported_functions);
bool generate_imported_global_variables(ArrayList *imported_global_variables);
bool evaluate_functions(ArrayList *functions);
bool evaluate_function(Function *function);
bool generate_enum_definitions(ArrayList *enum_definitions);
bool generate_enum_definition(Enum *enum_definition);

// statements
bool generate_statement(Statement *statement);

bool generate_expression_statement(ExpressionStatement *expr_stmt, Scope *scope);
bool generate_conditional_statement(ConditionalStatement *conditional_stmt, Scope *scope);
bool generate_loop_statement(LoopStatement *loop_stmt, Scope *scope);
bool generate_return_statement(ReturnStatement *return_statement, Scope *scope);
bool generate_block_statement(CompoundStatement *assembly_code_block, Scope *scope);
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt, Scope *scope);
bool generate_assembly_code_block_statement(AssemblyCodeBlockStatement *block_stmt, Scope *scope);

// expressions
bool generate_expression(Expression *expression, Register reg, Scope *scope);

bool generate_literal_expression(Literal *literal, Register reg, Scope *scope);
bool generate_unary_expression(UnaryExpression *unary_expr, Register reg, Scope *scope);
bool generate_binary_expression(BinaryExpression *binary_expr, Register out_reg, Scope *scope);
bool generate_functioncall_expression(FunctionCallExpression *func_call_expr, Register out_reg, Scope *scope);
bool generate_assignment_expression(AssignmentExpression *assignment_expr, Register out_reg, Scope *scope);
bool generate_array_access_expression(ArrayAccessExpression *array_index_expr, Register out_reg, Scope *scope);
bool generate_member_access_expression(MemberAccessExpression *member_access_expr, Register out_reg, Scope *scope);
bool generate_list_expression(ExpressionList *expr_list, Register out_reg, Scope *scope);

// utility functions
char *generate_local_variable_pointer(size_t rbp_offset, size_t size);
char *generate_variable_pointer(char *name, Scope *scope);
char *generate_operand_for_variable(Variable *variable, Scope *scope);

// ------------------------------------------------------------------------

// will be initialized and freed by 'generate_x86_64_assembly'
AssemblyProgram *assembly = NULL;

// will be reseted each time 'evaluate_function' is called
RegisterLayout *register_layout = NULL;

// will be reseted each time 'evaluate_function' is called
StackLayout *stack_layout = NULL;

// DataTypeTable *dtt = NULL;

bool setup_stack_frame = false;

// ------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param ast 
 * @param options 
 * @return char* 
 */
char *generate_x86_64_assembly(AST *ast, DataTypeTable *_dtt, CommandlineOptions *options) {
	log_debug("Generating x86-64 assembly code...\n");
	// TODO

	assembly = assembly_program_new();
	// dtt = _dtt;

	for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		log_debug(" - generating package '%s' ", package->name);

		if (!evaluate_package(package)) {
			printf(IRED "x" RESET "\n");
			
			// free allocated memory
			assembly_program_free(assembly);
			register_layout_free(register_layout);
			freeStackLayout(stack_layout);

			return NULL;
		}

#if defined DEBUG
		printf(GREEN "✓" RESET "\n");
#endif
	}

	// assembly.toString()
	// convert AssemblyProgram to string (char *)
	log_debug(" - assembly.toString() ");
	char *assembly_code = assembly_program_to_string(assembly);
	if (assembly_code == NULL) {
		printf(IRED "x" RESET "\n");
		
		// free allocated memory
		assembly_program_free(assembly);
		register_layout_free(register_layout);
		freeStackLayout(stack_layout);

		return NULL;
	} 
#if defined DEBUG
	printf(GREEN "✓" RESET "\n");
#endif
	// free allocated memory
	assembly_program_free(assembly);

	return assembly_code;

	// return compile_to_x86_64_assembly(ast, options);
}

/**
 * @brief 
 * 
 * @param main_func_template 
 */
void add__start_function(Function *main_func) {
	char *main_func_lcc_identifier = function_as_lcc_identifier(main_func);
	DEFINE_GLOBAL("_start");
	ADD_TEXT_LABEL("_start");
	ADD_INST("call", main_func_lcc_identifier);
	ADD_INST("mov", "rdi", "rax");
	ADD_INST("mov", "rax", "60");
	ADD_INST("syscall");
	free(main_func_lcc_identifier);
}

/**
 * @brief 
 * 
 * @param package 
 * @return true 
 */
bool evaluate_package(Package *package) {
	// printf(" - evaluating package '%s'\n", package->name);
	
	if (!evaluate_global_variables(package->global_variables)) {
		return false;
	}

	if (!evaluate_functions(package->functions)) {
		return false;
	}

	if (!generate_extern_functions(package->extern_functions)) {
		return false;
	}

	if (!generate_imported_functions(package->imported_functions)) {
		return false;
	}

	if (!generate_imported_global_variables(package->imported_global_variables)) {
		return false;
	}

	if (!generate_enum_definitions(package->enum_definitions)) {
		return false;
	}

	return true;
}

/**
 * @brief 
 * 
 * @param global_variables 
 * @return true 
 */
bool evaluate_global_variables(ArrayList *global_variables) {
	for (size_t i = 0; i < global_variables->size; i++) {
		Variable *variable = arraylist_get(global_variables, i);
		
		if (!evaluate_global_variable(variable)) {
			return false;
		}
	}
	return true;
}

/**
 * @brief 
 * 
 * @param variable 
 * @return true 
 */
bool evaluate_global_variable(Variable *variable) {
	char *lcc_identifier = variable_as_lcc_identifier(variable);
	DEFINE_GLOBAL(lcc_identifier);

	if (variable->initializer == NULL) {
		// add to .bss section
		bss_section_declare_space(
				assembly->bss,		// BssSection
				lcc_identifier, 	// label
				(AssemblyBssSectionTypes) variable->type->size,				// type - TODO(lucalewin): get correct directive for variable type
				variable->type->is_array ? variable->type->array_size : 1);	// count
	} else {
		// TODO: preevaluate the constant

		switch(variable->initializer->type) {
			case EXPRESSION_TYPE_LITERAL: {
				Literal *literal = variable->initializer->expr.literal;

				if (literal->type == LITERAL_STRING) {
					// add the string to the string table
					string_table_add(assembly->string_table, literal->value);
					string_table_add_label(assembly->string_table, literal->value, lcc_identifier);
				} else {
					// add to .data section
					DECLARE_VARIABLE(
							lcc_identifier,
							variable->type->size,
							literal->value);
				}
				break;
			}
			case EXPRESSION_TYPE_LIST: {
				ExpressionList *list = variable->initializer->expr.list;

				size_t buffer_size = 1;
				
				for (size_t i = 0; i < arraylist_size(list->expressions); i++) {
					Expression *expr = arraylist_get(list->expressions, i);
					
					if (expr->type != EXPRESSION_TYPE_LITERAL) {
						printf("%s:%d:%d: " RED "error: " RESET "initializer list must be a list of literals\n",
								"TODO", 0, 0);
						free(lcc_identifier);
						return false;
					}

					Literal *literal = expr->expr.literal;

					if (literal->type == LITERAL_STRING || literal->type == LITERAL_IDENTIFIER) {
						printf("%s:%d:%d: " RED "error: " RESET "list of strings or variables are currently not supported\n",
								"TODO", 0, 0);
						free(lcc_identifier);
						return false;
					}

					if (buffer_size == 0) {
						buffer_size += snprintf(NULL, 0, "%s", literal->value);
					} else {
						buffer_size += snprintf(NULL, 0, ",%s", literal->value);
					}
				}

				char *list_str = calloc(buffer_size, sizeof(char));

				int index = 0;
				for (size_t i = 0; i < arraylist_size(list->expressions); i++) {
					Expression *expr = arraylist_get(list->expressions, i);
					
					if (index == 0) {
						index += snprintf(list_str + index, buffer_size - index, "%s", expr->expr.literal->value);
					} else {
						index += snprintf(list_str + index, buffer_size - index, ",%s", expr->expr.literal->value);
					}
				}

				DECLARE_VARIABLE(
						lcc_identifier,
						variable->type->size,
						list_str);

				free(list_str);

				break;
			}
			default:
				printf("%s:%d:%d: " RED "error: " RESET "unsupported expression type '%d'\n",
						"TODO", 0, 0, variable->initializer->type);
				free(lcc_identifier);
				return false;
		}
	}

	free(lcc_identifier);

	return true;
}

/**
 * @brief 
 * 
 * @param extern_functions 
 * @return true  
 */
bool generate_extern_functions(ArrayList *extern_functions) {
	for (size_t i = 0; i < arraylist_size(extern_functions); i++) {
		Function *extern_function = arraylist_get(extern_functions, i);
		char *extern_func_lcc_ident = function_as_lcc_identifier(extern_function);

		DEFINE_EXTERN(extern_func_lcc_ident);
		
		free(extern_func_lcc_ident);
	}

	return true;
}

/**
 * @brief 
 * 
 * @param imported_functions 
 * @return true 
 * @return false 
 */
bool generate_imported_functions(ArrayList *imported_functions) {
	for (size_t i = 0; i < arraylist_size(imported_functions); i++) {
		Function *imported_function = arraylist_get(imported_functions, i);
		char *imported_func_lcc_ident = function_as_lcc_identifier(imported_function);

		DEFINE_EXTERN(imported_func_lcc_ident);

		free(imported_func_lcc_ident);
	}
	return true;
}

/**
 * @brief 
 * 
 * @param imported_global_variables 
 * @return true 
 * @return false 
 */
bool generate_imported_global_variables(ArrayList *imported_global_variables) {
	for (size_t i = 0; i < arraylist_size(imported_global_variables); i++) {
		Variable *imported_global_variable = arraylist_get(imported_global_variables, i);
		// char *imported_global_var_lcc_ident = variabletemplate_toLCCIdentifier(imported_global_variable);
		char *imported_global_var_lcc_ident = variable_as_lcc_identifier(imported_global_variable);
		DEFINE_EXTERN(imported_global_var_lcc_ident);
		free(imported_global_var_lcc_ident);
	}
	return true;
}

/**
 * @brief 
 * 
 * @param functions 
 * @return true 
 */
bool evaluate_functions(ArrayList *functions) {
	// printf(" - evaluating functions\n");
	for (size_t i = 0; i < functions->size; i++) {
		Function *function = arraylist_get(functions, i);

		// printf(" %s\n", function->identifier);

		if (!evaluate_function(function)) {
			return false;
		}
		if (strcmp(function->identifier, "main") == 0) {
			// add _func_main
			// Function *main_func = convert_to_function_template(function);
			add__start_function(function);
			// function_template_free(main_func);
		}
	}
	return true;
}

/**
 * @brief 
 * 
 * @param function 
 * @return true 
 */
bool evaluate_function(Function *function) {
	// initialize register/stack layout
	register_layout = register_layout_new();
	stack_layout = stack_layout_new();
	setup_stack_frame = false;

	// add global directive + label of function (as lcc identifier)
	char *lcc_ident = function_as_lcc_identifier(function);

	DEFINE_GLOBAL(lcc_ident);
	ADD_TEXT_LABEL(lcc_ident);

	free(lcc_ident);

	// add prologue
	if (arraylist_size(function->parameters) > 6) {
		for (size_t i = arraylist_size(function->parameters) - 1; i > 5; i--) {
			Variable *variable = arraylist_get(function->parameters, i);
			// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
			// FIXME: if var is a pointer or an array set size to 8
			stack_pushVariable(stack_layout, variable->identifier, variable->type->size);
		}
	}

	if (arraylist_size(function->parameters) > 0) {
		Variable *variable;
		// DataType *data_type;

		// 	rdi	rsi	rdx	rcx	r8	r9
		switch (arraylist_size(function->parameters)) {
			case 6:
				variable = arraylist_get(function->parameters, 5);
				register_setVariable(register_layout, REGISTER_R9, variable->type->size, variable->identifier);
			case 5:
				variable = arraylist_get(function->parameters, 4);
				register_setVariable(register_layout, REGISTER_R8, variable->type->size, variable->identifier);
			case 4:
				variable = arraylist_get(function->parameters, 3);
				register_setVariable(register_layout, REGISTER_RCX, variable->type->size, variable->identifier);
			case 3:
				variable = arraylist_get(function->parameters, 2);
				register_setVariable(register_layout, REGISTER_RDX, variable->type->size, variable->identifier);
			case 2:
				variable = arraylist_get(function->parameters, 1);
				register_setVariable(register_layout, REGISTER_RSI, variable->type->size, variable->identifier);
			case 1:
				variable = arraylist_get(function->parameters, 0);
				register_setVariable(register_layout, REGISTER_RDI, variable->type->size, variable->identifier);
				break;
			default:
				log_error("Too many parameters for function '%s'.\n", function->identifier);
				return false;
		}
	}

	size_t bytes_to_allocate = 0;
	if (arraylist_size(function->scope->local_variables) > 0) {
		setup_stack_frame = true;

		ADD_INST("push", "rbp");
		ADD_INST("mov", "rbp", "rsp");
		stack_pushRegister(stack_layout, REGISTER_RBP, 8, register_layout);

		for (size_t i = 0; i < arraylist_size(function->scope->local_variables); i++) {
			Variable *variable = arraylist_get(function->scope->local_variables, i);
			// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);

			size_t size = variable->type->is_array
							|| variable->type->is_pointer
							? 8 : variable->type->size;

			bytes_to_allocate += size;

			// printf(" - allocating %s (%zu bytes)\n", variable->identifier, size);

			stack_pushVariable(stack_layout, variable->identifier, size);
		}

		char *bytes_to_allocate_label = int_to_string(bytes_to_allocate);
		
		ADD_INST("sub", "rsp", bytes_to_allocate_label);
		free(bytes_to_allocate_label);

		// https://lucr4ft.github.io/luvascript-compiler/compiler/conventions/x64/calling-convention/#parameter-passing
		// arg0 | arg1 | arg2 | arg3 | arg4 | arg5 | argN
		// rdi  | rsi  | rdx  | rcx  | r8   | r9   | stack
		Register param_regs[] = {REGISTER_RDI, REGISTER_RSI, REGISTER_RDX, REGISTER_RCX, REGISTER_R8, REGISTER_R9};
		unsigned int index = min(arraylist_size(function->parameters), 6);

		for (size_t i = 0; i < index; i++) {
			Variable *variable = arraylist_get(function->scope->local_variables, i);
			// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
			char *var = generate_variable_pointer(variable->identifier, function->scope);
			size_t size = variable->type->is_array
							|| variable->type->is_pointer
							? 8 : variable->type->size;
			ADD_INST("mov", var, register_toString(param_regs[i], size));
			free(var);
		}

		// TODO: add support for more than 6 parameters

	}

	// generate statements
	for (size_t i = 0; i < arraylist_size(function->statements); i++) {
		Statement *statement = arraylist_get(function->statements, i);
		if (!generate_statement(statement)) {
			return false;
		}
	}

	// end of function: return (if function is void)
	// DataType *return_type = data_type_table_get(dtt, function->return_type);
	if (function->return_type->type == DATA_TYPE_VOID) {
		if (setup_stack_frame) {
			ADD_INST("leave");
			// TODO: clear stack
		}
		ADD_INST("ret");
	}

	register_layout_free(register_layout);
	freeStackLayout(stack_layout);
	return true;
}

bool generate_enum_definitions(ArrayList *enum_definitions) {
	for (size_t i = 0; i < arraylist_size(enum_definitions); i++) {
		Enum *enum_definition = arraylist_get(enum_definitions, i);
		if (!generate_enum_definition(enum_definition)) {
			return false;
		}
	}
	return true;
}

bool generate_enum_definition(Enum *enum_definition) {
	char *enum_lcc_identifer = enum_definition_as_lcc_identifier(enum_definition);
	DECLARE_VARIABLE(enum_lcc_identifer, 0, NULL); // just the label

	for (size_t i = 0; i < arraylist_size(enum_definition->members); i++) {
		EnumDefinitionMember *member = arraylist_get(enum_definition->members, i);
		char *member_lcc_identifer = enum_definition_member_as_lcc_identifier(member);
		char *value = int_to_string(member->value);
		DECLARE_VARIABLE(member_lcc_identifer, DATATYPE_DIRECTIVE_DWORD, value);
		free(member_lcc_identifer);
		free(value);
	}

	// printf("enum_definition: %s\n", enum_lcc_identifer);
	free(enum_lcc_identifer);

	return true;
}

// --------------------------------------- [ Statements ] ---------------------------------------

/**
 * @brief 
 * 
 * @param statement 
 * @return true 
 */
bool generate_statement(Statement *statement) {

	// printf("Statement: %d\n", statement->type);

	switch (statement->type) {
		case STATEMENT_EXPRESSION:
			return generate_expression_statement(statement->stmt.expression, statement->scope);
		case STATEMENT_CONDITIONAL:
			return generate_conditional_statement(statement->stmt.conditional, statement->scope);
		case STATEMENT_LOOP:
			return generate_loop_statement(statement->stmt.loop, statement->scope);
		case STATEMENT_RETURN:
			return generate_return_statement(statement->stmt._return, statement->scope);
		case STATEMENT_COMPOUND:
			return generate_block_statement(statement->stmt.compound, statement->scope);
		case STATEMENT_VARIABLE_DECLARATION:
			return generate_declaration_statement(statement->stmt.variable_declaration, statement->scope);
		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			return generate_assembly_code_block_statement(statement->stmt.assembly, statement->scope);
	}

	return false;
}

/**
 * @brief 
 * 
 * @param expr_stmt 
 * @return true 
 * @return false 
 */
bool generate_expression_statement(ExpressionStatement *expr_stmt, Scope *scope) {
	bool success = generate_expression(expr_stmt->expression, REGISTER_RAX, scope);

	// clear all registers
	register_clearAll(register_layout);

	return success;
}

/**
 * @brief 
 * 
 * @param conditional_stmt 
 * @return true 
 * @return false 
 */
bool generate_conditional_statement(ConditionalStatement *conditional_stmt, Scope *scope) {
	static unsigned int conditional_label_counter = 0;
	static unsigned int conditional_false_label_counter = 0;
	// static unsigned int conditional_end_label_generated = false;

	size_t conditional_end_label_size = snprintf(NULL, 0, ".CE%u", conditional_label_counter);
	char *conditional_end_label = malloc(conditional_end_label_size + 1);
	snprintf(conditional_end_label, conditional_end_label_size + 1, ".CE%u", conditional_label_counter);

	size_t conditional_false_label_size = snprintf(NULL, 0, ".CF%u", conditional_false_label_counter);
	char *conditional_false_label = malloc(conditional_false_label_size + 1);
	snprintf(conditional_false_label, conditional_false_label_size + 1, ".CF%u", conditional_false_label_counter);

	// if (!conditional_end_label_generated) {
	// 	conditional_end_label_generated = true;
	// }

	conditional_label_counter++;
	conditional_false_label_counter++;

	// generate condition
	if (!generate_expression(conditional_stmt->condition, REGISTER_RAX, scope)) {
		return false;
	}

	// jump to end if condition is false
	ADD_INST("cmp", register_toString(REGISTER_RAX, 8), "0");
	ADD_INST("je", conditional_stmt->false_branch == NULL ? conditional_end_label : conditional_false_label);

	// clear all registers (they were used for condition and may not have been cleared)
	register_clearAll(register_layout);

	// generate true statement
	if (!generate_statement(conditional_stmt->true_branch)) {
		return false;
	}

	if (conditional_stmt->false_branch != NULL) {
		// jump to end if condition is true
		ADD_INST("jmp", conditional_end_label);
		ADD_TEXT_LABEL(conditional_false_label);
		free(conditional_false_label);
		// conditional_false_label_counter++;

		// generate false statement
		if (!generate_statement(conditional_stmt->false_branch)) {
			return false;
		}
	}
	
	ADD_TEXT_LABEL(conditional_end_label);
	free(conditional_end_label);
	// conditional_label_counter++;

	// if (conditional_end_label_generated) {
	// 	ADD_TEXT_LABEL(conditional_end_label);
	// 	free(conditional_end_label);
	// 	conditional_label_counter++;
	// 	conditional_end_label_generated = false;
	// }

	return true; // FIXME
}

/**
 * @brief 
 * 
 * @param loop_stmt 
 * @return true 
 * @return false 
 */
bool generate_loop_statement(LoopStatement *loop_stmt, Scope *scope) {
	static unsigned int loop_counter = 0;

	size_t loop_label_length = snprintf(NULL, 0, ".LC%u", loop_counter);
	char *loop_label = calloc(loop_label_length + 1, sizeof(char));
	sprintf(loop_label, ".LC%u", loop_counter);

	size_t loop_end_label_length = snprintf(NULL, 0, ".LE%u", loop_counter);
	char *loop_end_label = calloc(loop_end_label_length + 1, sizeof(char));
	sprintf(loop_end_label, ".LE%u", loop_counter);

	// FUTURE OPTIMIZATION: check if the binary should be optimized for size or speed

	ADD_TEXT_LABEL(loop_label);
	// generate loop condition
	
	if (!generate_expression(loop_stmt->condition, REGISTER_RAX, scope)) {
		return false;
	}

	// if the condition is false, jump to the end of the loop
	ADD_INST("cmp", register_toString(REGISTER_RAX, 8), "0");
	ADD_INST("je", loop_end_label);

	// clear all registers
	register_clearAll(register_layout);

	// generate loop body
	if (!generate_statement(loop_stmt->body)) {
		return false;
	}

	// jump to the start of the loop
	ADD_INST("jmp", loop_label);

	// end of loop label
	ADD_TEXT_LABEL(loop_end_label);

	loop_counter++;

	return true; // FIXME
}

/**
 * @brief 
 * 
 * @param return_statement 
 * @return true 
 * @return false 
 */
bool generate_return_statement(ReturnStatement *return_statement, Scope *scope) {
	if (return_statement->expression != NULL) {
		if (!generate_expression(return_statement->expression, REGISTER_RAX, scope)) {
			return false;
		}
	}

	if (setup_stack_frame) {
		ADD_INST("leave");
	}

	ADD_INST("ret");

	return true;
}

/**
 * @brief 
 * 
 * @param block_stmt 
 * @return true 
 * @return false 
 */
bool generate_block_statement(CompoundStatement *block_stmt, Scope *scope) {

	// check if new variables are declared in the block
	if (arraylist_size(block_stmt->local_scope->local_variables) > 0) {
		// calculate the size of the stack frame for the new variables
		size_t stack_frame_size = 0;
		
		for (size_t i = 0; i < arraylist_size(block_stmt->local_scope->local_variables); i++) {
			Variable *variable = arraylist_get(block_stmt->local_scope->local_variables, i);
			// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
			stack_frame_size += variable->type->size;
			stack_pushVariable(stack_layout, variable->identifier, variable->type->size);
		}

		// convert the stack frame size to a string
		size_t stack_frame_str_length = snprintf(NULL, 0, "%zu", stack_frame_size);
		char *stack_frame_size_str = calloc(stack_frame_str_length + 1, sizeof(char));
		sprintf(stack_frame_size_str, "%zu", stack_frame_size);

		// allocate space for the new variables
		setup_stack_frame = true;
		ADD_INST("sub", register_toString(REGISTER_RSP, 8), stack_frame_size_str);

		free(stack_frame_size_str);
	}

	for (size_t i = 0; i < arraylist_size(block_stmt->nested_statements); i++) {
		Statement *statement = arraylist_get(block_stmt->nested_statements, i);
		if (!generate_statement(statement)) {
			return false;
		}
	}

	return true;
}

/**
 * @brief 
 * 
 * @param decl_stmt 
 * @return true 
 * @return false 
 */
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt, Scope *scope) {
	if (decl_stmt->variable->initializer != NULL) {
		if (!generate_expression(decl_stmt->variable->initializer, REGISTER_RAX, scope)) {
			printf("Error: could not generate default value for variable %s\n", decl_stmt->variable->identifier);
			return false;
		}
	}
	
	// get the position of the variable on the stack
	size_t var_rbp_offset = stack_getVariableOffset(stack_layout, decl_stmt->variable->identifier);
	size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

	if ((signed int)var_rbp_offset == -1 || (signed int)var_size == -1) {
		return false;
	}

	// printf("    " IRED "error: " RESET "generate_declaration_statement not implemented\n");
	// printf("var size: %d\n", (signed int)var_size);
	const char *size_directive = _AssemblyDataType_directives[var_size];

	size_t len = snprintf(NULL, 0, "%s[rbp-%zu]", size_directive, var_rbp_offset);

	char *var_pointer = calloc(len + 1, sizeof(char));

	sprintf(var_pointer, "%s[rbp-%zu]", size_directive, var_rbp_offset);

	ADD_INST("mov", var_pointer, register_toString(REGISTER_RAX, var_size));

	register_clear(register_layout, REGISTER_RAX);

	free(var_pointer);

	return true;
}

/**
 * @brief 
 * 
 * @param assembly_code_block 
 * @param scope 
 * @return true 
 * @return false 
 */
bool generate_assembly_code_block_statement(AssemblyCodeBlockStatement *assembly_code_block, Scope *scope) {

	// TODO: parse assembly + check if assembly is valid
	// (1) split assembly code into lines
	// (2) trim lines
	// (3) check if line is empty
	// (4) add line as instruction

	ADD_INST(assembly_code_block->code);

	return true;
}

// ------------------------------------------- EXPRESSIONS -------------------------------------------

/**
 * @brief 
 * 
 * @param expression 
 * @param reg 
 * @return true 
 * @return false 
 */
bool generate_expression(Expression *expression, Register reg, Scope *scope) {
	switch (expression->type) {
		case EXPRESSION_TYPE_LITERAL:
			return generate_literal_expression(expression->expr.literal, reg, scope);
		case EXPRESSION_TYPE_UNARY:
			return generate_unary_expression(expression->expr.unary, reg, scope);
		case EXPRESSION_TYPE_BINARY:
			return generate_binary_expression(expression->expr.binary, reg, scope);
		case EXPRESSION_TYPE_NESTED:
			return generate_expression(expression->expr.nested, reg, scope);
		case EXPRESSION_TYPE_FUNCTION_CALL:
			return generate_functioncall_expression(expression->expr.function_call, reg, scope);
		case EXPRESSION_TYPE_ASSIGNMENT:
			return generate_assignment_expression(expression->expr.assignment, reg, scope);
		case EXPRESSION_TYPE_ARRAY_ACCESS:
			return generate_array_access_expression(expression->expr.array_access, reg, scope);
		case EXPRESSION_TYPE_MEMBER_ACCESS:
			return generate_member_access_expression(expression->expr.member_access, reg, scope);
		case EXPRESSION_TYPE_LIST:
			return generate_list_expression(expression->expr.list, reg, scope);
		default:
			// print error
			log_error("Unsupported expression type: %d\n", expression->type);
			return false;
	}
	return false;
}

bool generate_literal_expression(Literal *literal, Register reg, Scope *scope) {
	// if type = string
	//		check if string is in string_table
	//			mov pointer to string into reg
	//		else
	//			add string to string_table
	//			mov pointer to string into reg
	// if type = number
	//		mov number into reg
	// if type = boolean
	//		mov boolean into reg
	// if type = variable
	// 		check if variable is already in register
	//			(if not)mov variable into reg

	switch (literal->type) {
		case LITERAL_NUMBER:
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()
			ADD_INST("mov", register_toString(reg, 8), literal->value);
			register_setValue(register_layout, reg, 8, literal->value);
			return true;
		case LITERAL_BOOLEAN:
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()
			ADD_INST("mov", register_toString(reg, 8), literal->value);
			register_setValue(register_layout, reg, 8, literal->value);
			return true;
		case LITERAL_CHARACTER: {
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()

			// convert the character to a string
			size_t len = snprintf(NULL, 0, "'%s'", literal->value);
			char *char_str = calloc(len + 1, sizeof(char));
			sprintf(char_str, "'%s'", literal->value);

			ADD_INST("mov", register_toString(reg, 8), char_str);
			register_setValue(register_layout, reg, 8, char_str);
			
			free(char_str);
			
			return true;
		}
		case LITERAL_IDENTIFIER: {
			// check if the variable is already stored in a register
			if (register_containsVariable(register_layout, literal->value)) {
				Register other = register_getVariable(register_layout, literal->value);
				ADD_INST("mov", register_toString(reg, 8), register_toString(other, 8));
				register_setVariable(register_layout, reg, 8, literal->value);
				return true;
			}

			// check if global scope contains the variable with the name of the identifier
			// if so, mov the value of the variable into the register
			if (scope_contains_global_variable(scope, literal->value)) {
				Variable *variable = scope_get_variable_by_name(scope, literal->value);
				// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);

				// char *lcc_ident = variabletemplate_toLCCIdentifier(variable);
				char *mnemonic = variable->type->size == 8 
								|| variable->type->is_array
								|| variable->type->is_pointer ? "mov" : "movsx";

				// char *var = generate_operand_for_variable(variable, scope);
				char *var = generate_variable_pointer(variable->identifier, scope);

				// printf("%s %s, %s\n", mnemonic, register_toString(reg, 8), var);

				ADD_INST(mnemonic, register_toString(reg, 8), var);
				register_setVariable(register_layout, reg, 8, literal->value);

				free(var);

				return true;
			}
			
			// else: the variable is a local variable stored in the stack

			// get the position of the variable on the stack
			size_t var_rbp_offset = stack_getVariableOffset(stack_layout, literal->value);
			size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

			if (var_rbp_offset == -1 || var_size == -1) {
				return false;
			}

			// get the size directive for the variable
			const char *var_size_directive = _AssemblyDataType_directives[var_size];

			// calculate the length of the string, then allocate it
			size_t len = snprintf(NULL, 0, "%s[rbp-%zu]", var_size_directive, var_rbp_offset);
			char *var_pointer = calloc(len + 1, sizeof(char));

			// create the string
			sprintf(var_pointer, "%s[rbp-%zu]", var_size_directive, var_rbp_offset);

			char *mnemonic = var_size == 8 ? "mov" : "movsx";

			// move the variable into the register
			ADD_INST(mnemonic, register_toString(reg, 8), var_pointer);
			register_setVariable(register_layout, reg, 8, literal->value);

			free(var_pointer);

			return true;
		}
		case LITERAL_STRING: {
			// check if the string is in the string_table
			if (!string_table_contains(assembly->string_table, literal->value)) {
				// add string to string_table
				string_table_add(assembly->string_table, literal->value);
			}

			// get the LNC identifer of the string
			char *lnc_ident = string_table_toLNCIdentifier(assembly->string_table, literal->value);

			// add instruction
			ADD_INST("mov", register_toString(reg, 8), lnc_ident);

			// set the register to the string
			register_setVariable(register_layout, reg, 8, literal->value);

			free(lnc_ident);

			return true;
		}
		default:
			// TODO: support other types
			// print error
			log_error("Unsupported literal type: %d\n", literal->type);
			return false;
	}
	return false;
}

bool generate_unary_expression(UnaryExpression *unary_expr, Register reg, Scope *scope) {
	switch (unary_expr->operator) {
		case UNARY_OPERATOR_NEGATE:
			// only variables can be negated
			// (numbers are already negated by the compiler)

			if (unary_expr->identifier->type != LITERAL_IDENTIFIER) {
				log_error("Unary operator '-' can only be applied to variables and number literals\n");
				return false;
			}

			// get the size of the variable
			char *mnemonic = NULL;
			char *operand = generate_variable_pointer(unary_expr->identifier->value, scope);

			if (scope_contains_global_variable(scope, unary_expr->identifier->value)) {
				Variable *variable = scope_get_variable_by_name(scope, unary_expr->identifier->value);
				// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
				
				mnemonic = variable->type->size == 8 ? "mov" : "movsx";
			} else {
				size_t var_rbp_offset = stack_getVariableOffset(stack_layout, unary_expr->identifier->value);
				size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

				if (var_rbp_offset == -1 || var_size == -1) {
					return false;
				}

				mnemonic = var_size == 8 ? "mov" : "movsx";
			}

			ADD_INST("neg", operand);
			ADD_INST(mnemonic, register_toString(reg, 8), operand);
			register_setValue(register_layout, reg, 8, NULL);

			free(operand);
			break;
		case UNARY_OPERATOR_INCREMENT: {

			// the identifier can only be a variable (because of the typechecker) but safety first
			if (unary_expr->identifier->type != LITERAL_IDENTIFIER) {
				log_error("Unsupported literal type: %d\n", unary_expr->identifier->type);
				return false;
			}

			char *mnemonic = NULL;
			char *value = NULL;

			if (scope_contains_global_variable(scope, unary_expr->identifier->value)) {
				Variable *variable = scope_get_variable_by_name(scope, unary_expr->identifier->value);
				// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
				mnemonic = variable->type->size == 8 ? "mov" : "movsx";
				value = generate_variable_pointer(variable->identifier, scope);
			} else {
				size_t var_rbp_offset = stack_getVariableOffset(stack_layout, unary_expr->identifier->value);
				size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

				if (var_rbp_offset == -1 || var_size == -1) {
					return false;
				}

				mnemonic = var_size == 8 ? "mov" : "movsx";
				value = generate_local_variable_pointer(var_rbp_offset, var_size);
			}

			ADD_INST("inc", value);
			ADD_INST(mnemonic, register_toString(reg, 8), value);
			register_setVariable(register_layout, reg, 8, unary_expr->identifier->value);

			free(value);

			break;
		}
		default:
			// print error
			log_error("Unsupported unary operator: %d\n", unary_expr->operator);
			return false;
	}

	return true;
}

bool generate_binary_expression(BinaryExpression *binary_expr, Register out_reg, Scope *scope) {

	/*


	Step 1: check if a register is empty
			set other to the empty register
			if no register is empty, set other and out_reg to -1

	Step 1.5: check if binary operator requires special registers (div -> eax:edx/e?x)

	Step 2: check if binary expression contains a literal expression
			if so, generate generate other expression and 

	Step 3: generate expression 1 and store result in other
			generate expression 2 and store result in out_reg

	*/

	// step 1
	Register other = -1;

	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		if (i == out_reg) {
			continue;
		}
		RegisterInfo *reg_info = register_layout_getRegisterInfo(register_layout, i);
		if (registerinfo_isEmpty(reg_info)) {
			other = i;
			break;
		}
	}

	size_t out_reg_size = 8;
	size_t reg_other_size = 8;
	// size_t special_reg_size = 8;

	// step 1.5
	// TODO: implement step 1.5
	bool pushed_rax = false;
	bool pushed_rdx = false;
	Register special_reg = -1;
	if (binary_expr->operator == BINARY_OPERATOR_DIVIDE || binary_expr->operator == BINARY_OPERATOR_MODULO) {
		other = REGISTER_RAX;
		if (out_reg == REGISTER_RAX) {
			for (size_t i = 0; i < REGISTER_COUNT; i++) {
				if (i == out_reg || i == REGISTER_RDX) {
					continue;
				}
				RegisterInfo *reg_info = register_layout_getRegisterInfo(register_layout, i);
				if (registerinfo_isEmpty(reg_info)) {
					special_reg = i;
					break;
				}
			}
		} else {
			special_reg = out_reg;
		}

		RegisterInfo *rax = register_layout_getRegisterInfo(register_layout, REGISTER_RAX);
		RegisterInfo *rdx = register_layout_getRegisterInfo(register_layout, REGISTER_RDX);

		// if rax is not empty, push the value in rax on the stack
		if (!registerinfo_isEmpty(rax)) {
			ADD_INST("push", register_toString(REGISTER_RAX, 8));
			pushed_rax = true;
			stack_pushRegister(stack_layout, REGISTER_RAX, 8, register_layout);
		}

		// if rdx is not empty, push the value in rdx on the stack
		if (!registerinfo_isEmpty(rdx)) {
			ADD_INST("push", register_toString(REGISTER_RDX, 8));
			pushed_rdx = true;
			stack_pushRegister(stack_layout, REGISTER_RDX, 8, register_layout);
		}
	}

	// step 2
	char *second_operand = NULL;
	bool free_second_operand = false;

	if (binary_expr->right->type == EXPRESSION_TYPE_LITERAL) {

		Literal *literal = binary_expr->right->expr.literal;
		Expression *other_expr = binary_expr->left;

		// generate other expression and store result in out_reg
		if (!generate_expression(other_expr, special_reg == -1 ? out_reg : other, scope)) {
			return false;
		}

		if (literal->type == LITERAL_IDENTIFIER) {
			// check if variable is already stored in a register
			if (register_containsVariable(register_layout, literal->value)) {
				Register other_reg = register_getVariable(register_layout, literal->value);
				second_operand = register_toString(other_reg, 8);
			} else if (scope_contains_global_variable(scope, literal->value)) {
				Variable *variable = scope_get_variable_by_name(scope, literal->value);
				// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
				char *var = generate_variable_pointer(literal->value, scope);
				second_operand = var;
				reg_other_size = variable->type->size;

			} else {
				size_t var_rbp_offset = stack_getVariableOffset(stack_layout, literal->value);
				size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

				if (var_rbp_offset == -1 || var_size == -1) {
					return false;
				}

				char *var_pointer = generate_local_variable_pointer(var_rbp_offset, var_size);
				// char *var_pointer = generate_variable_pointer(literal->value, scope);

				reg_other_size = var_size;
				second_operand = var_pointer;
			}
		} else if (literal->type == LITERAL_CHARACTER) {
			const char *format = "%d";
			size_t length = snprintf(NULL, 0, format, (int) *(literal->value)) + 1;
			char *value = malloc(length);
			sprintf(value, format, (int) *(literal->value));
			
			second_operand = value;
			free_second_operand = true;
		} else {
			second_operand = literal->value;
		}
	} else {
		// step 3
		// generate left expression
		if (!generate_expression(
					binary_expr->left,
					special_reg == -1 ? out_reg : other,
					scope)) {
			return false;
		}

		// generate right expression
		if (!generate_expression(
					binary_expr->right,
					special_reg == -1 ? other : special_reg,
					scope)) {
			return false;
		}

		second_operand = register_toString(other, 8);
	}

	// add binary instruction with other and out_reg as registers
	switch (binary_expr->operator) {
		case BINARY_OPERATOR_ADD:
			ADD_INST("add", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_SUBTRACT:
			ADD_INST("sub", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_MULTIPLY:
			ADD_INST("imul", register_toString(out_reg, reg_other_size), second_operand);
			break;
			// FIXME: see step 1.5
		case BINARY_OPERATOR_DIVIDE:
			if (binary_expr->right->type == EXPRESSION_TYPE_LITERAL) {
				ADD_INST("mov", register_toString(special_reg == -1 ? other : special_reg, reg_other_size), second_operand);
			}

			ADD_INST("xor", register_toString(REGISTER_RDX, 8), register_toString(REGISTER_RDX, 8));

			ADD_INST("idiv", register_toString(special_reg == -1 ? other : special_reg, reg_other_size));

			if (out_reg != REGISTER_RAX) {
				ADD_INST("mov", register_toString(out_reg, out_reg_size), register_toString(REGISTER_RAX, out_reg_size));
			}

			if (pushed_rax) {
				ADD_INST("pop", register_toString(REGISTER_RAX, 8));
			}
			if (pushed_rdx) {
				ADD_INST("pop", register_toString(REGISTER_RDX, 8));
			}
			break;
		case BINARY_OPERATOR_MODULO:
			if (binary_expr->right->type == EXPRESSION_TYPE_LITERAL) {
				ADD_INST("mov", register_toString(special_reg == -1 ? other : special_reg, 8), second_operand);
			}
			ADD_INST("xor", register_toString(REGISTER_RDX, 8), register_toString(REGISTER_RDX, 8));
			ADD_INST("idiv", register_toString(special_reg == -1 ? other : special_reg, 8));
			if (out_reg != REGISTER_RDX) {
				ADD_INST("mov", register_toString(out_reg, out_reg_size), register_toString(REGISTER_RDX, 8));
			}
			if (pushed_rax) {
				ADD_INST("pop", register_toString(REGISTER_RAX, 8));
			}
			if (pushed_rdx) {
				ADD_INST("pop", register_toString(REGISTER_RDX, 8));
			}
			break;
		case BINARY_OPERATOR_LOGICAL_EQUAL:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("sete", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_LOGICAL_NOT_EQUAL:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("setne", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_LOGICAL_LESS:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("setl", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("setle", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_LOGICAL_GREATER:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("setg", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL:
			ADD_INST("cmp", register_toString(out_reg, reg_other_size), second_operand);
			ADD_INST("setge", register_toString(out_reg, 1));
			break;
		case BINARY_OPERATOR_BITWISE_AND:
			ADD_INST("and", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_BITWISE_OR:
			ADD_INST("or", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_BITWISE_XOR:
			ADD_INST("xor", register_toString(out_reg, reg_other_size), second_operand);
			break;
		// FIXME: implement bitwise not
		// case BINARY_OPERATOR_BITWISE_NOT:
		// 	ADD_INST("not", register_toString(out_reg, 8), register_toString(out_reg, 8));
		// 	break;
		case BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT:
			ADD_INST("shl", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT:
			ADD_INST("shr", register_toString(out_reg, reg_other_size), second_operand);
			break;
		// FIXME: implement bitwise unsigned arithmetic right shift
		// case BINARY_OPERATOR_BITWISE_SHIFT_RIGHT_UNSIGNED:
		// 	ADD_INST("sar", register_toString(out_reg, 8), second_operand);
		// 	break;
		case BINARY_OPERATOR_LOGICAL_AND:
			ADD_INST("and", register_toString(out_reg, reg_other_size), second_operand);
			break;
		case BINARY_OPERATOR_LOGICAL_OR:
			ADD_INST("or", register_toString(out_reg, reg_other_size), second_operand);
			break;
		default:
			// print error
			log_error("Unsupported binary operator: %d\n", binary_expr->operator);
			return false;
	}

	// value = NULL, because it its not needed and most of the time it is known at compile time
	register_setValue(register_layout, out_reg, out_reg_size, NULL);

	// clear other if it was set
	if (other != -1) {
		register_clear(register_layout, other);
	}

	if (free_second_operand) {
		free(second_operand);
	}

	return true;
}

bool generate_functioncall_expression(FunctionCallExpression *func_call_expr, Register out_reg, Scope *scope) {
	// FIXME: floating point types are currently not allowed as function paramenters

	// rdi	rsi	rdx	rcx	r8	r9 ... stack

	bool pushed_rax = false;

	if (!register_isEmpty(register_layout, REGISTER_RAX)) {
		ADD_INST("push", register_toString(REGISTER_RAX, 8));
		pushed_rax = true;
	}

	Expression *expr = NULL;

	switch (arraylist_size(func_call_expr->argument_expression_list->expressions)) {
		case 6:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 5);
			if (!generate_expression(expr, REGISTER_R9, scope)) {
				return false;
			}
		case 5:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 4);
			if (!generate_expression(expr, REGISTER_R8, scope)) {
				return false;
			}
		case 4:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 3);
			if (!generate_expression(expr, REGISTER_RCX, scope)) {
				return false;
			}
		case 3:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 2);
			if (!generate_expression(expr, REGISTER_RDX, scope)) {
				return false;
			}
		case 2:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 1);
			if (!generate_expression(expr, REGISTER_RSI, scope)) {
				return false;
			}
		case 1:
			expr = arraylist_get(func_call_expr->argument_expression_list->expressions, 0);
			if (!generate_expression(expr, REGISTER_RDI, scope)) {
				return false;
			}
			break;
		case 0:
			break;
		default:
			log_error("Too many arguments in function call: %d\n", arraylist_size(func_call_expr->argument_expression_list->expressions));
			return false;
	}

	// FunctionTemplate *func = scope_get_function_by_name(scope, func_call_expr->function_identifier);
	Function *func = scope_getFunctionByID(scope, func_call_expr->id);

	// printf("FUNC: %s - %llu\n", func->identifier, func_call_expr->id);

	char *lcc_ident = function_as_lcc_identifier(func);

	// call function
	ADD_INST("call", lcc_ident);

	free(lcc_ident);

	// DataType *return_type = data_type_table_get(dtt, func->return_type);

	// set output register
	register_setValue(register_layout, out_reg, 0, NULL); // FIXME: replace 0 with actual size of return type

	// clear registers
	register_clear(register_layout, REGISTER_RDI);
	register_clear(register_layout, REGISTER_RSI);
	register_clear(register_layout, REGISTER_RDX);
	register_clear(register_layout, REGISTER_RCX);
	register_clear(register_layout, REGISTER_R8);
	register_clear(register_layout, REGISTER_R9);

	if (out_reg != REGISTER_RAX) {
		ADD_INST("mov", register_toString(out_reg, 8), register_toString(REGISTER_RAX, 8));
	}

	if (pushed_rax) {
		ADD_INST("pop", register_toString(REGISTER_RAX, 8));
	}

	return true;
}

bool generate_assignment_expression(AssignmentExpression *assignment_expr, Register out_reg, Scope *scope) {

	if (assignment_expr->identifier->type != EXPRESSION_TYPE_LITERAL) {
		log_error("Assignment to non-literal identifier not supported\n");
		return false;
	}
	
	Register special_reg = -1;

	if (assignment_expr->operator == ASSIGNMENT_OPERATOR_DIVIDE) {
		// TODO: add modulo assignment operator
		if (out_reg == REGISTER_RAX) {
			for (size_t i = 0; i < REGISTER_COUNT; i++) {
				if (i == out_reg || i == REGISTER_RDX) {
					continue;
				}
				if (register_isEmpty(register_layout, i)) {
					special_reg = i;
					break;
				}
			}
		} else {
			;;
		}
	}

	if (!generate_expression(
			assignment_expr->assignment_value,
			special_reg == -1 ? out_reg : special_reg,
			scope)) {
		return false;
	}
	
	register_setValue(register_layout, special_reg == -1 ? out_reg : special_reg, 8, NULL);

	// get variable
	Literal *literal = assignment_expr->identifier->expr.literal;

	if (literal->type != LITERAL_IDENTIFIER) {
		log_error("Assignment to non-identifier literal not supported\n");
		return false;
	}

	size_t var_rbp_offset = stack_getVariableOffset(stack_layout, literal->value);
	size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

	if (var_rbp_offset == -1 || var_size == -1) {
		log_error("Variable %s not found\n", literal->value);
		return false;
	}

	char *var_pointer = generate_local_variable_pointer(var_rbp_offset, var_size);

	// add assignement instruction
	switch (assignment_expr->operator) {
		case ASSIGNMENT_OPERATOR_DEFAULT:
			ADD_INST("mov", var_pointer, register_toString(out_reg, var_size));
			break;
		case ASSIGNMENT_OPERATOR_ADD:
			ADD_INST("add", var_pointer, register_toString(out_reg, var_size));
			break;
		case ASSIGNMENT_OPERATOR_SUBTRACT:
			ADD_INST("sub", var_pointer, register_toString(out_reg, var_size));
			break;
		case ASSIGNMENT_OPERATOR_MULTIPLY: {
			// find an empty register
			Register reg = register_getEmpty(register_layout);
			if (reg == -1) {
				log_error("No empty register available\n");
				return false;
			}

			// load variable into register
			ADD_INST("mov", register_toString(reg, var_size), var_pointer);
			ADD_INST("imul", register_toString(reg, var_size), register_toString(out_reg, var_size));
			ADD_INST("mov", var_pointer, register_toString(reg, var_size));

			// ADD_INST("imul", var_pointer, register_toString(out_reg, var_size));
			break;
		}
		case ASSIGNMENT_OPERATOR_DIVIDE: {
			bool pushed_rax = false;
			if (!register_isEmpty(register_layout, REGISTER_RAX)) {
				ADD_INST("push", register_toString(REGISTER_RAX, 8));
				stack_pushRegister(stack_layout, REGISTER_RAX, 8, register_layout);
				pushed_rax = true;
			}

			ADD_INST("mov", register_toString(REGISTER_RAX, var_size), var_pointer);
			ADD_INST("idiv", register_toString(special_reg == -1 ? out_reg : special_reg, var_size));
			ADD_INST("mov", var_pointer, register_toString(REGISTER_RAX, var_size));

			if (pushed_rax) {
				ADD_INST("pop", register_toString(REGISTER_RAX, 8));
				stack_popRegister(stack_layout, REGISTER_RAX, 8, register_layout);
			}
			break;
		}
		default:
			// print error
			log_error("Unsupported assignment operator: %d\n", assignment_expr->operator);
			return false;
	}

	return true;
}

bool generate_array_access_expression(ArrayAccessExpression *array_index_expr, Register out_reg, Scope *scope) {

	if (array_index_expr->identifier->type != LITERAL_IDENTIFIER) {
		log_error("Array access to non-literal identifier not supported\n");
		return false;
	}
	
	Register index_expression_register = register_getEmpty(register_layout);

	if (index_expression_register == -1) {
		log_error("No empty register available\n");
		return false;
	}

	if (!generate_expression(
			array_index_expr->index_expression,
			index_expression_register,
			scope)) {
		return false;
	}

	Variable *variable = scope_get_variable_by_name(scope, array_index_expr->identifier->value);
	// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);
	char *mnemonic = variable->type->size == 8 ? "mov" : "movsx";

	if (!variable->type->is_array && !variable->type->is_pointer) {
		// printf("VAR: %s\n", variable->identifier);
		log_error("Array access to non-array variable not supported\n");
		return false;
	}

	if (scope_contains_local_variable(scope, array_index_expr->identifier->value)) {
		// local variable
		size_t var_rbp_offset = stack_getVariableOffset(stack_layout, array_index_expr->identifier->value);

		if (var_rbp_offset == -1) {
			log_error("Variable %s not found\n", array_index_expr->identifier->value);
			return false;
		}

		Register pointer_register = register_getEmpty(register_layout);
		
		// mov pointer_register, qword[rbp-X] ; --> the pointer
		// mov out-reg, qword[pointer_register+index_expression_register*size] ; --> the value

		// using 8 because the pointer is always 8 bytes
		char *var_pointer = generate_local_variable_pointer(var_rbp_offset, 8);
		ADD_INST("mov", register_toString(pointer_register, 8), var_pointer);

		const char *format = "%s[%s+%s*%d]";
		const char *datatype_directive = _AssemblyDataType_directives[variable->type->size];
		const char *reg = register_toString(pointer_register, 8);
		const char *index_reg = register_toString(index_expression_register, 8);
		const int array_offset = variable->type->size;

		size_t length = snprintf(NULL, 0, format, datatype_directive, reg, index_reg, array_offset) + 1;
		char *instruction = calloc(length, sizeof(char));
		sprintf(instruction, format, datatype_directive, reg, index_reg, array_offset);

		ADD_INST(mnemonic, register_toString(out_reg, 8), instruction);

		free(instruction);
	} else {
		// global variable
		const char *format = "%s[%s+%d*%s]";
		const char *datatype_directive = _AssemblyDataType_directives[variable->type->size];
		char *lcc_identifier = variable_as_lcc_identifier(variable);
		const char *register_str = register_toString(index_expression_register, 8);

		size_t source_operand_length = snprintf(NULL, 0, format, datatype_directive,
					lcc_identifier, variable->type->size, register_str);
		char *source_operand = calloc(source_operand_length + 1, sizeof(char));
		sprintf(source_operand, format, datatype_directive,
				lcc_identifier, variable->type->size, register_str);


		ADD_INST(mnemonic, register_toString(out_reg, 8), source_operand);

		free(lcc_identifier);
		free(source_operand);
	}

	return true;
}

bool generate_member_access_expression(MemberAccessExpression *member_access_expr, Register out_reg, Scope *scope) {
	// printf("Generating access to member\n");
	
	// DataType *member_access_expr->datatype = expression_get_datatype(member_access_expr);

	if (member_access_expr->datatype == NULL) {
		log_error("Could not get datatype of member access expression\n");
		return false;
	}

	if (member_access_expr->datatype->is_array || member_access_expr->datatype->is_pointer) {
		log_error("Member access to array or pointer not supported\n");
		return false;
	}

	if (member_access_expr->datatype->type != DATA_TYPE_ENUM) {
		printf("%s:%d:%d: " RED "error: " RESET "member access to non-enum type not supported\n",
				"TODO", 0, 0);
		return false;
	}

	if (member_access_expr->identifier->type != EXPRESSION_TYPE_LITERAL) {
		log_error("Member access to non-literal identifier not supported\n");
		return false;
	}

	Literal *identifer_literal = member_access_expr->identifier->expr.literal;

	if (identifer_literal->type != LITERAL_IDENTIFIER) {
		log_error("Member access to non-identifier literal not supported\n");
		return false;
	}

	Enum *enum_def = scope_get_enum_by_name(scope, identifer_literal->value);
	EnumDefinitionMember *enum_member = enum_definition_member_get(enum_def, member_access_expr->member_identifier);

	const char *dt_directive = _AssemblyDataType_directives[ENUM_MEMBER_SIZE_BYTES];
	char *enum_def_ident = enum_definition_as_lcc_identifier(enum_def);
	char *enum_member_ident = enum_definition_member_as_lcc_identifier(enum_member);


	char *format = "%s[%s%s]";
	size_t length = snprintf(NULL, 0, format, dt_directive, enum_def_ident, enum_member_ident) + 1;
	char *instruction = calloc(length, sizeof(char));
	sprintf(instruction, format, dt_directive, enum_def_ident, enum_member_ident);

	ADD_INST("mov", register_toString(out_reg, ENUM_MEMBER_SIZE_BYTES), instruction);

	free(enum_def_ident);
	free(enum_member_ident);

	return true;
}

bool generate_list_expression(ExpressionList *expr_list, Register out_reg, Scope *scope) {
	return false; // TODO
}

char *generate_operand_for_variable(Variable *variable, Scope *scope) {
	// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);

	size_t variable_rbp_offset = stack_getVariableOffset(stack_layout, variable->identifier);
	
	if (variable_rbp_offset != -1) {
		// the variable is on the stack --> its a local variable

		size_t var_size = stack_getItemSize(stack_layout, variable_rbp_offset);

		if (var_size == -1) {
			return NULL;
		}

		// get the size directive for the variable
		const char *var_size_directive = _AssemblyDataType_directives[var_size];

		// calculate the length of the string, then allocate it
		size_t len = snprintf(NULL, 0, "%s[rbp-%zu]", var_size_directive, variable_rbp_offset);
		char *var_pointer = calloc(len + 1, sizeof(char));

		// create the string
		sprintf(var_pointer, "%s[rbp-%zu]", var_size_directive, variable_rbp_offset);

		return var_pointer;
	}


	if (scope_contains_global_variable(scope, variable->identifier)) {
		char *lcc_identifier = variable_as_lcc_identifier(variable);

		if (variable->type->is_pointer || variable->type->is_array) {
			// the variable is a global pointer or array which means the var_address is a label
			// Therefore we need to convert the address to an lcc identifier
			return lcc_identifier;
		}
		// the variable is not a pointer or an array and can be directly dereferenced

		size_t buffer_size = strlen(_AssemblyDataType_directives[variable->type->size])
				+ strlen(lcc_identifier) + 2;
		char *value = calloc(buffer_size + 1, sizeof(char));

		strcpy(value, _AssemblyDataType_directives[variable->type->size]);
		strcat(value, "[");
		strcat(value, lcc_identifier);
		strcat(value, "]");

		free(lcc_identifier);

		return value;
	}

	return NULL;
}

char *generate_variable_pointer(char *name, Scope *scope) {
	// if the stack contains the variable
	//   --> return a local address (rbp-offset)
	// else, the variable is a global variable
	//   --> return the global address of the variable (label)

	size_t variable_rbp_offset = stack_getVariableOffset(stack_layout, name);

	if (variable_rbp_offset != -1) {
		// the variable is on the stack --> its a local variable
		size_t var_size = stack_getItemSize(stack_layout, variable_rbp_offset);
		if (var_size == -1) {
			return NULL;
		}
		char *var_pointer = generate_local_variable_pointer(variable_rbp_offset, var_size);
		return var_pointer;
	} else {
		Variable *variable = scope_get_variable_by_name(scope, name);
		if (variable == NULL) {
			return NULL;
		}

		// DataType *data_type = data_type_table_get(dtt, variable->type_identifier);

		char *lcc_identifier = variable_as_lcc_identifier(variable);

		if (variable->type->is_pointer || variable->type->is_array) {
			// the variable is a global pointer or array which means the var_address is a label
			// Therefore we need to convert the address to an lcc identifier
			return lcc_identifier;
		}

		// the variable is not a pointer or an array and can be directly dereferenced
		size_t buffer_size = strlen(_AssemblyDataType_directives[variable->type->size])
				+ strlen(lcc_identifier) + 2;
		char *value = calloc(buffer_size + 1, sizeof(char));
		strcpy(value, _AssemblyDataType_directives[variable->type->size]);
		strcat(value, "[");
		strcat(value, lcc_identifier);
		strcat(value, "]");
		free(lcc_identifier);
		return value;
	}
	
	return NULL;
}

char *generate_local_variable_pointer(size_t rbp_offset, size_t size) {
	if (rbp_offset == -1 || size == -1) {
		return NULL;
	}

	// get the size directive for the variable
	const char *var_size_directive = _AssemblyDataType_directives[size];

	// calculate the length of the string, then allocate it
	size_t len = snprintf(NULL, 0, "%s[rbp-%zu]", var_size_directive, rbp_offset);
	char *var_pointer = calloc(len + 1, sizeof(char));

	// create the string
	sprintf(var_pointer, "%s[rbp-%zu]", var_size_directive, rbp_offset);

	return var_pointer;
}
