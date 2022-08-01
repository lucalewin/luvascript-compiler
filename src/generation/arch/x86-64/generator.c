#include <generation/arch/x86-64/generator.h>

#include <stdbool.h>

#include <util/util.h>

// #include <x86-64/compiler.h>
#include <conventions/lcc.h>

#include <parsing/scope_impl.h>
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

bool generate_expression_statement(ExpressionStatement *expr_stmt, Scope *scope);
bool generate_conditional_statement(ConditionalStatement *conditional_stmt);
bool generate_loop_statement(LoopStatement *loop_stmt);
bool generate_return_statement(ReturnStatement *return_statement, Scope *scope);
bool generate_block_statement(CompoundStatement *block_stmt);
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt, Scope *scope);

// expressions
bool generate_expression(Expression_T *expression, Register reg, Scope *scope);

bool generate_literal_expression(Literal_T *literal, Register reg, Scope *scope);
bool generate_unary_expression(UnaryExpression_T *unary_expr, Register reg, Scope *scope);
bool generate_binary_expression(BinaryExpression_T *binary_expr, Register out_reg, Scope *scope);
bool generate_functioncall_expression(FunctionCallExpression_T *func_call_expr, Register out_reg, Scope *scope);
bool generate_assignment_expression(AssignmentExpression_T *assignment_expr, Register out_reg, Scope *scope);

// utility functions
char *generate_local_variable_pointer(size_t rbp_offset, size_t size);
char *generate_variable_pointer(char *name, Scope *scope);
char *generate_operand_for_variable(VariableTemplate *variable, Scope *scope);

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
			freeStackLayout(stack_layout);

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
		freeStackLayout(stack_layout);

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
				(AssemblyBssSectionTypes) variable->type->size,				// type - TODO(lucalewin): get correct directive for variable type
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
		DECLARE_VARIABLE(label, (AssemblyDataSectionTypes) variable->type->size, value);

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
		for (size_t i = arraylist_size(function->parameters) - 1; i > 5; i--) {
			Variable *variable = arraylist_get(function->parameters, i);
			// FIXME: if var is a pointer or an array set size to 8
			stack_pushVariable(stack_layout, variable->identifier->value, variable->type->size);
		}
	}

	if (arraylist_size(function->parameters) > 0) {
		Variable *var;

		// 	rdi	rsi	rdx	rcx	r8	r9
		switch (arraylist_size(function->parameters)) {
			case 6:
				var = arraylist_get(function->parameters, 5);
				register_setVariable(register_layout, REGISTER_R9, var->type->size, var->identifier->value);
			case 5:
				var = arraylist_get(function->parameters, 4);
				register_setVariable(register_layout, REGISTER_R8, var->type->size, var->identifier->value);
			case 4:
				var = arraylist_get(function->parameters, 3);
				register_setVariable(register_layout, REGISTER_RCX, var->type->size, var->identifier->value);
			case 3:
				var = arraylist_get(function->parameters, 2);
				register_setVariable(register_layout, REGISTER_RDX, var->type->size, var->identifier->value);
			case 2:
				var = arraylist_get(function->parameters, 1);
				register_setVariable(register_layout, REGISTER_RSI, var->type->size, var->identifier->value);
			case 1:
				var = arraylist_get(function->parameters, 0);
				register_setVariable(register_layout, REGISTER_RDI, var->type->size, var->identifier->value);
				break;
			default:
				log_error("Too many parameters for function '%s'.\n", function->identifier);
				return false;
		}
	}

	size_t bytes_to_allocate = 0;
	if (arraylist_size(function->scope->local_variable_templates) > 0) {
		setup_stack_frame = true;

		ADD_INST("push", "rbp");
		ADD_INST("mov", "rbp", "rsp");
		stack_pushRegister(stack_layout, REGISTER_RBP, 8, register_layout);

		for (size_t i = 0; i < arraylist_size(function->scope->local_variable_templates); i++) {
			VariableTemplate *variable_template = arraylist_get(function->scope->local_variable_templates, i);
			bytes_to_allocate += variable_template->datatype->size;

			stack_pushVariable(stack_layout, variable_template->identifier, variable_template->datatype->size);
		}

		char *bytes_to_allocate_label = int_to_string(bytes_to_allocate);
		
		ADD_INST("sub", "rsp", bytes_to_allocate_label);

		// print size of the stack
		// printf("\nstack-size: %ld\n", stack_getSize(stack_layout));

		free(bytes_to_allocate_label);
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
			// TODO: clear stack
		}
		ADD_INST("ret");
	}

	register_layout_free(register_layout);
	freeStackLayout(stack_layout);
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

	switch (statement->type) {
		case STATEMENT_EXPRESSION:
			return generate_expression_statement(statement->stmt.expression_statement, statement->scope);
		case STATEMENT_CONDITIONAL:
			return generate_conditional_statement(statement->stmt.conditional_statement);
		case STATEMENT_LOOP:
			return generate_loop_statement(statement->stmt.loop_statement);
		case STATEMENT_RETURN:
			return generate_return_statement(statement->stmt.return_statement, statement->scope);
		case STATEMENT_COMPOUND:
			return generate_block_statement(statement->stmt.compound_statement);
		case STATEMENT_VARIABLE_DECLARATION:
			return generate_declaration_statement(statement->stmt.variable_decl, statement->scope);
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
bool generate_declaration_statement(VariableDeclarationStatement *decl_stmt, Scope *scope) {
	if (decl_stmt->variable->default_value != NULL) {
		if (!generate_expression(decl_stmt->variable->default_value, REGISTER_RAX, scope)) {
			return false;
		}
	}
	
	// get the position of the variable on the stack
	size_t var_rbp_offset = stack_getVariableOffset(stack_layout, decl_stmt->variable->identifier->value);
	size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

	if (var_rbp_offset == -1 || var_size == -1) {
		return false;
	}

	const char *size_directive = _AssemblyDataType_directives[var_size];

	size_t len = snprintf(NULL, 0, "%s[rbp-%zu]", size_directive, var_rbp_offset);

	char *var_pointer = calloc(len + 1, sizeof(char));

	sprintf(var_pointer, "%s[rbp-%zu]", size_directive, var_rbp_offset);

	ADD_INST("mov", var_pointer, register_toString(REGISTER_RAX, var_size));

	register_clear(register_layout, REGISTER_RAX);

	free(var_pointer);

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
bool generate_expression(Expression_T *expression, Register reg, Scope *scope) {
	switch (expression->type) {
		case EXPRESSION_TYPE_LITERAL:
			return generate_literal_expression(expression->expr.literal_expr, reg, scope);
		case EXPRESSION_TYPE_UNARY:
			return generate_unary_expression(expression->expr.unary_expr, reg, scope);
		case EXPRESSION_TYPE_BINARY:
			return generate_binary_expression(expression->expr.binary_expr, reg, scope);
		case EXPRESSION_TYPE_NESTED:
			return generate_expression(expression->expr.nested_expr->expression, reg, scope);
		case EXPRESSION_TYPE_FUNCTIONCALL:
			return generate_functioncall_expression(expression->expr.func_call_expr, reg, scope);
		case EXPRESSION_TYPE_ASSIGNMENT:
			return generate_assignment_expression(expression->expr.assignment_expr, reg, scope);
		default:
			// print error
			log_error("Unsupported expression type: %d\n", expression->type);
			return false;
	}
	return false;
}

bool generate_literal_expression(Literal_T *literal, Register reg, Scope *scope) {
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
		case LITERAL_CHARACTER:
			// FIXME: replace 8 with size of the number
			// see typechecker.c : type_of_literal_expression()
			ADD_INST("mov", register_toString(reg, 8), literal->value);
			register_setValue(register_layout, reg, 8, literal->value);
			return true;
		case LITERAL_IDENTIFIER:
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
				VariableTemplate *variable = scope_get_variable_by_name(scope, literal->value);

				// char *lcc_ident = variabletemplate_toLCCIdentifier(variable);
				char *mnemonic = variable->datatype->size == 8 ? "mov" : "movsx";

				// char *var = generate_operand_for_variable(variable, scope);
				char *var = generate_variable_pointer(variable->identifier, scope);

				printf("%s %s, %s\n", mnemonic, register_toString(reg, 8), var);

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
		default:
			// TODO: support other types
			// print error
			log_error("Unsupported literal type: %d\n", literal->type);
			return false;
	}
	return false;
}

bool generate_unary_expression(UnaryExpression_T *unary_expr, Register reg, Scope *scope) {
	// if literal is a number and the operator is - --> move the number into the register then negate it

	// if literal is a boolean and the operator is ! --> mov opposite of boolean into reg

	char *operand = NULL;

	switch (unary_expr->identifier->type) {
		case LITERAL_NUMBER:
		case LITERAL_BOOLEAN:
		case LITERAL_CHARACTER:
			ADD_INST("mov", register_toString(reg, 8), unary_expr->identifier->value);
			register_setValue(register_layout, reg, 8, unary_expr->identifier->value);
			operand = register_toString(reg, 8);
			break;

		case LITERAL_IDENTIFIER: {

			char *var = generate_operand_for_variable(scope_get_variable_by_name(scope, unary_expr->identifier->value), scope);

			size_t var_rbp_offset = stack_getVariableOffset(stack_layout, unary_expr->identifier->value);
			size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

			if (var_rbp_offset == -1 || var_size == -1) {
				return false;
			}

			char *mnemonic = var_size == 8 ? "mov" : "movsx";

			ADD_INST(mnemonic, register_toString(reg, 8), var);
			register_setValue(register_layout, reg, 8, var);
			operand = register_toString(reg, 8);

			free(var);
			break;
		}
		
		default:
			// print error
			log_error("Unsupported literal type: %d\n", unary_expr->identifier->type);
			return false;
	}

	switch (unary_expr->operator) {
		case UNARY_OPERATOR_NEGATE:
			ADD_INST("neg", operand);
			break;
		case UNARY_OPERATOR_INCREMENT:
			ADD_INST("inc", operand);
			break;
		default:
			// print error
			log_error("Unsupported unary operator: %d\n", unary_expr->operator);
			return false;
	}

	return true;
}

bool generate_binary_expression(BinaryExpression_T *binary_expr, Register out_reg, Scope *scope) {

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
			// TODO: push the StackLayout + set value in RegisterLayout
		}

		// if rdx is not empty, push the value in rdx on the stack
		if (!registerinfo_isEmpty(rdx)) {
			ADD_INST("push", register_toString(REGISTER_RDX, 8));
			pushed_rdx = true;
			// TODO
		}
	}


	// step 2
	char *second_operand = NULL;

	if (binary_expr->expression_right->type == EXPRESSION_TYPE_LITERAL) {

		Literal_T *literal = binary_expr->expression_right->expr.literal_expr;
		Expression_T *other_expr = binary_expr->expression_left;

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
				VariableTemplate *var_template = scope_get_variable_by_name(scope, literal->value);
				char *var = generate_variable_pointer(literal->value, scope);
				second_operand = var;
				reg_other_size = var_template->datatype->size;

			} else {
				size_t var_rbp_offset = stack_getVariableOffset(stack_layout, literal->value);
				size_t var_size = stack_getItemSize(stack_layout, var_rbp_offset);

				if (var_rbp_offset == -1 || var_size == -1) {
					return false;
				}

				char *var_pointer = generate_local_variable_pointer(var_rbp_offset, var_size);
				// char *var_pointer = generate_variable_pointer(literal->value, scope);

				printf("VAR_POINTER: %s\n", var_pointer);

				reg_other_size = var_size;
				second_operand = var_pointer;
			}
		} else {
			second_operand = literal->value;
		}
	} else {
		// step 3
		// generate left expression
		if (!generate_expression(
					binary_expr->expression_left,
					special_reg == -1 ? out_reg : other,
					scope)) {
			return false;
		}

		// generate right expression
		if (!generate_expression(
					binary_expr->expression_right,
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
			if (binary_expr->expression_right->type == EXPRESSION_TYPE_LITERAL) {
				ADD_INST("mov", register_toString(special_reg == -1 ? other : special_reg, reg_other_size), second_operand);
			}

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
			if (binary_expr->expression_right->type == EXPRESSION_TYPE_LITERAL) {
				ADD_INST("mov", register_toString(special_reg == -1 ? other : special_reg, 8), second_operand);
			}
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

	return true;
}

bool generate_functioncall_expression(FunctionCallExpression_T *func_call_expr, Register out_reg, Scope *scope) {
	// FIXME: floating point types are currently not allowed as function paramenters

	// rdi	rsi	rdx	rcx	r8	r9 ... stack

	bool pushed_rax = false;

	if (!register_isEmpty(register_layout, REGISTER_RAX)) {
		ADD_INST("push", register_toString(REGISTER_RAX, 8));
		pushed_rax = true;
	}

	Expression_T *expr = NULL;

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

	FunctionTemplate *func = scope_get_function_by_name(scope, func_call_expr->function_identifier);

	char *lcc_ident = functiontemplate_as_lcc_identifier(func);

	// call function
	ADD_INST("call", lcc_ident);

	free(lcc_ident);

	// set output register
	register_setValue(register_layout, out_reg, func->return_type->size, NULL);

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

bool generate_assignment_expression(AssignmentExpression_T *assignment_expr, Register out_reg, Scope *scope) {
	printf("generate_assignment_expression\n");
	
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
	Literal_T *literal = assignment_expr->identifier->expr.literal_expr;

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

	printf("assignment operator %d\n", assignment_expr->operator);

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

char *generate_operand_for_variable(VariableTemplate *variable, Scope *scope) {

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
		char *lcc_identifier = variabletemplate_toLCCIdentifier(variable);

		if (variable->datatype->is_pointer || variable->datatype->is_array) {
			// the variable is a global pointer or array which means the var_address is a label
			// Therefore we need to convert the address to an lcc identifier
			return lcc_identifier;
		}
		// the variable is not a pointer or an array and can be directly dereferenced

		size_t buffer_size = strlen(_AssemblyDataType_directives[variable->datatype->size])
				+ strlen(lcc_identifier) + 2;
		char *value = calloc(buffer_size + 1, sizeof(char));

		strcpy(value, _AssemblyDataType_directives[variable->datatype->size]);
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
		VariableTemplate *variable = scope_get_variable_by_name(scope, name);
		if (variable == NULL) {
			return NULL;
		}

		char *lcc_identifier = variabletemplate_toLCCIdentifier(variable);

		if (variable->datatype->is_pointer || variable->datatype->is_array) {
			// the variable is a global pointer or array which means the var_address is a label
			// Therefore we need to convert the address to an lcc identifier
			return lcc_identifier;
		}

		// the variable is not a pointer or an array and can be directly dereferenced
		size_t buffer_size = strlen(_AssemblyDataType_directives[variable->datatype->size])
				+ strlen(lcc_identifier) + 2;
		char *value = calloc(buffer_size + 1, sizeof(char));
		strcpy(value, _AssemblyDataType_directives[variable->datatype->size]);
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

// char *generate_global_variable_pointer(char *name) {
// 	return NULL;
// }
