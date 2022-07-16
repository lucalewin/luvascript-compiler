#include <generation/arch/x86-64/generator.h>

#include <stdbool.h>

#include <util/util.h>

// #include <x86-64/compiler.h>
#include <conventions/lcc.h>

#include <parsing/nodes/package.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/literal.h>

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

void add__start_function(FunctionTemplate *main_func_template);

bool evaluate_package(Package *package);
bool evaluate_global_variables(ArrayList *global_variables);
bool evaluate_global_variable(Variable *variable);
bool generate_extern_functions(ArrayList *extern_functions);
bool evaluate_functions(ArrayList *functions);
bool evaluate_function(Function *function);


// statements
bool generate_statement(Statement *statement);

bool generate_expression_statement(ExpressionStatement *expr_stmt);
bool generate_conditional_statement(ConditionalStatement *conditional_stmt);
bool generate_loop_statement(LoopStatement *loop_stmt);
bool generate_return_statement(ReturnStatement *return_statement);
bool generate_block_statement(CompoundStatement *block_stmt);
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt);

// expressions
bool generate_expression(Expression_T *expression, Register reg);

bool generate_literal_expression(Literal_T *literal, Register reg);

// ------------------------------------------------------------------------

// will be initialized and freed by 'generate_x86_64_assembly'
AssemblyProgram *assembly = NULL;

// will be reseted each time 'evaluate_function' is called
RegisterLayout *register_layout = NULL;

// will be reseted each time 'evaluate_function' is called
StackLayout *stack_layout = NULL;

bool setup_stack_frame = false;

// ------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param ast 
 * @param options 
 * @return char* 
 */
char *generate_x86_64_assembly(AST *ast, CommandlineOptions *options) {
	log_debug("Generating x86-64 assembly code...\n");
	// TODO

	assembly = assembly_program_new();


	for (size_t i = 0; i < arraylist_size(ast->packages); i++) {
		Package *package = arraylist_get(ast->packages, i);

		log_debug(" - generating package '%s' ", package->name);

		if (!evaluate_package(package)) {
			printf(IRED "x" RESET "\n");
			
			// free allocated memory
			assembly_program_free(assembly);
			register_layout_free(register_layout);
			stack_layout_free(stack_layout);

			return NULL;
		}

		printf(GREEN "✓" RESET "\n");
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
		stack_layout_free(stack_layout);

		return NULL;
	} 

	printf(GREEN "✓" RESET "\n");

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
void add__start_function(FunctionTemplate *main_func_template) {
	char *main_func_lcc_identifier = function_to_lcc_identifier(main_func_template);
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
	if (!evaluate_global_variables(package->global_variables)) {
		return false;
	}

	if (!evaluate_functions(package->functions)) {
		return false;
	}

	if (!generate_extern_functions(package->extern_functions)) {
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
	if (variable->default_value == NULL) {
		// add to .bss section
		bss_section_declare_space(
				assembly->bss,					// BssSection
				variable->identifier->value, 	// label
				BSS_TYPE_RESB, 					// type - TODO(lucalewin): get correct directive for variable type
				variable->type->is_array ? variable->type->array_size : 1);	// count
	} else {
		// TODO: preevaluate the constant
		// just for testing purposes
		if (variable->default_value->type != EXPRESSION_TYPE_LITERAL) {
			log_error("Default value of global variable '%s' is not a literal.\n", variable->identifier->value);
			return false;
		}
		
		Literal_T *literal = variable->default_value->expr.literal_expr;
		char *value = NULL;

		if (literal->type == LITERAL_STRING) {
			// surround with ` quotes
			value = malloc(strlen(literal->value) + 3);
			sprintf(value, "`%s`", literal->value);
		} else {
			value = strdup(literal->value);
		}

		char *label = variable_as_lcc_identifier(variable);

		// add to .data section
		DECLARE_VARIABLE(label, DATA_TYPE_DB, value);

		free(label);
		free(value);
	}

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
		FunctionTemplate *extern_function = arraylist_get(extern_functions, i);
		char *extern_func_lcc_ident = function_to_lcc_identifier(extern_function);
		
		DEFINE_EXTERN(extern_func_lcc_ident);
		
		free(extern_func_lcc_ident);
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
	for (size_t i = 0; i < functions->size; i++) {
		Function *function = arraylist_get(functions, i);

		if (!evaluate_function(function)) {
			return false;
		}
		if (strcmp(function->identifier, "main") == 0) {
			// add _func_main
			FunctionTemplate *main_func_template = convert_to_function_template(function);
			add__start_function(main_func_template);
			function_template_free(main_func_template);
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
		setup_stack_frame = true;

		ADD_INST("mov", "rbp", "rsp");

		for (size_t i = arraylist_size(function->parameters) - 1; i > 5; i--) {
			Variable *variable = arraylist_get(function->parameters, i);
			stack_push_variable(stack_layout, variable->identifier->value);
		}
	}

	// generate statements
	for (size_t i = 0; i < arraylist_size(function->body_statements); i++) {
		Statement *statement = arraylist_get(function->body_statements, i);
		if (!generate_statement(statement)) {
			return false;
		}
	}

	// end of function: return (if function is void)
	if (strcmp(function->return_type->type_identifier, "void") == 0) {
		if (setup_stack_frame) {
			ADD_INST("leave");
		}
		ADD_INST("ret");
	}

	register_layout_free(register_layout);
	stack_layout_free(stack_layout);
	return true;
}

/**
 * @brief 
 * 
 * @param statement 
 * @return true 
 */
bool generate_statement(Statement *statement) {

	switch (statement->type) {
		case STATEMENT_EXPRESSION:
			return generate_expression_statement(statement->stmt.expression_statement);
		case STATEMENT_CONDITIONAL:
			return generate_conditional_statement(statement->stmt.conditional_statement);
		case STATEMENT_LOOP:
			return generate_loop_statement(statement->stmt.loop_statement);
		case STATEMENT_RETURN:
			return generate_return_statement(statement->stmt.return_statement);
		case STATEMENT_COMPOUND:
			return generate_block_statement(statement->stmt.compound_statement);
		case STATEMENT_VARIABLE_DECLARATION:
			return generate_declaration_statement(statement->stmt.variable_decl);
		case STATEMENT_ASSEMBLY_CODE_BLOCK:
			return false;
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
bool generate_expression_statement(ExpressionStatement *expr_stmt) {
	return generate_expression(expr_stmt->expression, REGISTER_RAX);
}

/**
 * @brief 
 * 
 * @param conditional_stmt 
 * @return true 
 * @return false 
 */
bool generate_conditional_statement(ConditionalStatement *conditional_stmt) {
	return false;
}

/**
 * @brief 
 * 
 * @param loop_stmt 
 * @return true 
 * @return false 
 */
bool generate_loop_statement(LoopStatement *loop_stmt) {
	return false;
}

/**
 * @brief 
 * 
 * @param return_statement 
 * @return true 
 * @return false 
 */
bool generate_return_statement(ReturnStatement *return_statement) {
	if (return_statement->expression != NULL) {
		if (!generate_expression(return_statement->expression, REGISTER_RAX)) {
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
bool generate_block_statement(CompoundStatement *block_stmt) {
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
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt) {
	return false;
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
bool generate_expression(Expression_T *expression, Register reg) {
	switch (expression->type) {
		case EXPRESSION_TYPE_LITERAL:
			return generate_literal_expression(expression->expr.literal_expr, reg);
		default:
			// print error
			log_error("Unsupported expression type: %d\n", expression->type);
			return false;
	}
	return false;
}

bool generate_literal_expression(Literal_T *literal, Register reg) {
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
			ADD_INST("mov", getRegisterWithOpCodeSize(reg, 8) ,literal->value);
			return true;
		case LITERAL_BOOLEAN:
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()
			ADD_INST("mov", getRegisterWithOpCodeSize(reg, 8), literal->value);
			return true;
		case LITERAL_CHARACTER:
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()
			ADD_INST("mov", getRegisterWithOpCodeSize(reg, 8), literal->value);
			return true;
		default:
			// TODO: support other types
			// print error
			log_error("Unsupported literal type: %d\n", literal->type);
			return false;
	}
	return false;
}