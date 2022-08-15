#include <parsing/parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/util.h>
#include <util/string.h>
#include <util/logging/logger.h>

#include <parsing/scope_impl.h>

#include <parsing/nodes/datatypes.h>
#include <parsing/nodes/expression.h>
#include <parsing/nodes/function.h>
#include <parsing/nodes/statement.h>
#include <parsing/nodes/variable.h>
#include <parsing/nodes/package.h>
#include <parsing/nodes/import.h>
#include <parsing/nodes/enum.h>

// global variables used by the parser
int _index;
ArrayList *tokens;
Token *current;
Token *lookahead;

static unsigned long long int function_id_counter = 0;

int expect(TokenType type) {
    if (current->type != type) {
        log_error("Expected token '%s' at [%d:%d] but got '%s' with value '%s' instead\n",
				TOKEN_TYPE_NAMES[type],
				current->line,
				current->pos,
				TOKEN_TYPE_NAMES[current->type],
				current->data);
        exit(1);
    }
    return 1;
}

int is(TokenType type) {
    if (current != NULL) {
        return current->type == type;
    } else {
        return 0;
    }
}

void next() {
    current = arraylist_get(tokens, _index++);
    if (_index < tokens->size) {
        lookahead = arraylist_get(tokens, _index);
    } else {
        lookahead = NULL;
    }
}

void eat(TokenType type) {
    expect(type);
    next();
}

void error(const char *msg) {
    log_error("an exception occured during parsing: %s\n", msg);
    exit(1);
}

int isAssignmentOperator(Token *t) {
    switch(t->type) {
        case TOKEN_ASSIGNMENT_SIMPLE:
        case TOKEN_ASSIGNMENT_SUM:
        case TOKEN_ASSIGNMENT_DIFFERENCE:
        case TOKEN_ASSIGNMENT_PRODUCT:
        case TOKEN_ASSIGNMENT_QUOTIENT:
        case TOKEN_ASSIGNMENT_REMAINDER:
        case TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT:
        case TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT:
        case TOKEN_ASSIGNMENT_BITWISE_AND:
        case TOKEN_ASSIGNMENT_BITWISE_XOR:
        case TOKEN_ASSIGNMENT_BITWISE_OR:
            return 1;
        default:
            return 0;
    }
}

int isUnaryOperator(Token *t) {
    switch(t->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_TILDE:
        case TOKEN_NOT:
            return 1;
        default:
            return 0;
    }
}

// -----------------------------------------------------------------------------------

// function prototypes

Package *expectPackage();
Variable *parseVariable();

Enum *parseEnumDefinition();
// StructDefinition *parseStructDefinition();

char *eatIdentifier();
bool expectKeyword(const char *keyword);

// -----------------------------------------------------------------------------------

const char *_filename;


Package *parse(ArrayList *token_list, const char *filename) {
	tokens = token_list;
    _index = 0;
	_filename = filename;

    // load tokens into 'current' and 'lookahead'
    next();

	log_debug("Parsing file '%s'\n", filename);

	Package *package = expectPackage();

	log_debug("Parsing finished\n");

    return package;
}

Package *expectPackage() {
	Package *package = calloc(1, sizeof(Package));

	package->functions = arraylist_create();
	package->extern_functions = arraylist_create();
	package->global_variables = arraylist_create();
	package->enum_definitions = arraylist_create();
	package->import_declarations = arraylist_create();
	package->imported_functions = arraylist_create();
	package->imported_global_variables = arraylist_create();

	if (is(TOKEN_KEYWORD)) {
		if (strcmp(current->data, "package") == 0) {
			eat(TOKEN_KEYWORD);

			if (!is(TOKEN_IDENTIFIER)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
				// free allocated memory
				package_free(package);
				return NULL;
			}
			package->name = strdup(current->data);
			eat(TOKEN_IDENTIFIER);

			while (is(TOKEN_DOT)) {
				eat(TOKEN_DOT);

				if (!is(TOKEN_IDENTIFIER)) {
					printf("%s:%d:%d: " RED "error: " RESET "expected package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
					// free allocated memory
					package_free(package);
					return NULL;
				}
				char *new_package_name = calloc(strlen(package->name) + strlen(current->data) + 2, sizeof(char));
				strcpy(new_package_name, package->name);
				strcat(new_package_name, ".");
				strcat(new_package_name, current->data);
				free(package->name);
				package->name = new_package_name;
				eat(TOKEN_IDENTIFIER);
			}

			eat(TOKEN_SEMICOLON);
		} else {
			goto default_package;
		}
	} else {
		default_package:
		package->name = strdup("global");
	}

	while (_index < tokens->size && is(TOKEN_KEYWORD) && strcmp(current->data, "import") == 0) {
		eat(TOKEN_KEYWORD); // import

		ImportDeclaration *import_declaration = import_declaration_new();
		// TODO: add NULL checks

		if (!is(TOKEN_IDENTIFIER)) {
			printf("%s:%d:%d: " RED "error: " RESET "expected package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
			// free allocated memory
			import_declaration_free(import_declaration);
			package_free(package);
			return NULL;
		}

		arraylist_add(import_declaration->package_names, strdup(current->data));
		import_declaration->package_name = strdup(current->data);
		eat(TOKEN_IDENTIFIER);

		// log_debug("current tokentype: %s\n", TOKEN_TYPE_NAMES[current->type]);

		while (is(TOKEN_DOT)) {
			eat(TOKEN_DOT);
			if (!is(TOKEN_IDENTIFIER)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
				// free allocated memory
				import_declaration_free(import_declaration);
				package_free(package);
				return NULL;
			}
			arraylist_add(import_declaration->package_names, strdup(current->data));
			char *new_package_name = calloc(strlen(import_declaration->package_name) + strlen(current->data) + 2, sizeof(char));
			strcpy(new_package_name, import_declaration->package_name);
			strcat(new_package_name, ".");
			strcat(new_package_name, current->data);
			free(import_declaration->package_name);
			import_declaration->package_name = new_package_name;
			eat(TOKEN_IDENTIFIER);
		}

		// log_warning("PACKAGE_NAME: %s\n", import_declaration->package_name);

		if (is(TOKEN_COLON)) {
			eat(TOKEN_COLON);
			
			if (!is(TOKEN_COLON)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected second ':' after package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
				// free allocated memory
				import_declaration_free(import_declaration);
				package_free(package);
				return NULL;
			}

			eat(TOKEN_COLON);

			// check if identifier or
			if (is(TOKEN_IDENTIFIER)) {
				arraylist_add(import_declaration->type_identifiers, strdup(current->data));
				eat(TOKEN_IDENTIFIER);
			} else if (is(TOKEN_ASTERISK)) {
				arraylist_add(import_declaration->type_identifiers, strdup(current->data));
				eat(TOKEN_ASTERISK); // *
			} else if (is(TOKEN_LBRACE)) {
				eat(TOKEN_LBRACE); // {
				while (is(TOKEN_IDENTIFIER)) {
					arraylist_add(import_declaration->type_identifiers, strdup(current->data));
					eat(TOKEN_IDENTIFIER);
					if (is(TOKEN_COMMA)) {
						eat(TOKEN_COMMA);
					} else {
						break;
					}
				}

				if (!is(TOKEN_RBRACE)) {
					printf("%s:%d:%d: " RED "error: " RESET "expected '}' after type identifiers, but got '%s'\n", _filename, current->line, current->pos, current->data);
					// free allocated memory
					import_declaration_free(import_declaration);
					package_free(package);
					return NULL;
				}

				eat(TOKEN_RBRACE); // }
			} else {
				printf("%s:%d:%d: " RED "error: " RESET "expected identifier or '*' or '{' after ':', but got '%s'\n", _filename, current->line, current->pos, current->data);
				// free allocated memory
				import_declaration_free(import_declaration);
				package_free(package);
				return NULL;
			}
		} else {
			if (!is(TOKEN_SEMICOLON)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected ';' after package name, but got '%s'\n", _filename, current->line, current->pos, current->data);
				// free allocated memory
				import_declaration_free(import_declaration);
				package_free(package);
				return NULL;
			}
			arraylist_add(import_declaration->type_identifiers, strdup("*"));
		}

		eat(TOKEN_SEMICOLON); // ;

		arraylist_add(package->import_declarations, import_declaration);
	}

	ArrayList *compacted_import_declarations = compact_import_declarations(package->import_declarations);

	if (compacted_import_declarations == NULL) {
		package_free(package);
		return NULL;
	}

	// free packages->import_declarations
	for (int i = 0; i < package->import_declarations->size; i++) {
		ImportDeclaration *import_declaration = (ImportDeclaration *) arraylist_get(package->import_declarations, i);
		import_declaration_free(import_declaration);
	}
	arraylist_free(package->import_declarations);
	package->import_declarations = compacted_import_declarations;

	// log_info("parsed all import statements\n");

	while (_index < tokens->size) {
		if (strcmp(current->data, "function") == 0) {
			Function *func = expectFunction();
			if (func == NULL) {
				package_free(package);
				log_error("error: " RESET " expected variable or function identifier, but got '%s'\n", current->data);
				return NULL;
			}
			arraylist_add(package->functions, func);
		} else if (strcmp(current->data, "var") == 0 || strcmp(current->data, "const") == 0) {
			// expect global variable/constant declaration
			Variable *glob_var = parseVariable();
			if (glob_var == NULL) {
				package_free(package);
				return NULL;
			}
			arraylist_add(package->global_variables, glob_var);
		} else if (strcmp(current->data, "extern") == 0) {
			
			// expect extern function declaration
			// Function *extern_func = expectExternFunction();
			// if (extern_func == NULL) {
			// 	// free allocated memory
			// 	package_free(package);
			// 	return NULL;
			// }
			// arraylist_add(package->extern_functions, extern_func);
			printf("%s:%d:%d: " RED "error: " RESET "extern functions are not supported yet\n", _filename, current->line, current->pos);
		} else if (strcmp(current->data, "enum") == 0) {
			Enum *enum_definition = parseEnumDefinition();
			// printf("%s:%d:%d: " RED "error: " RESET "enums are not supported\n", _filename, current->line, current->pos);
			if (enum_definition == NULL) {
				// free allocated memory
				package_free(package);
				return NULL;
			}
			arraylist_add(package->enum_definitions, enum_definition);
		} else {
			printf("%s:%d:%d: " RED "error: " RESET "unexpected token '%s'\n", _filename, current->line, current->pos, current->data);
			package_free(package);
			return NULL;
		}
	}

	return package;
}

// Function *expectExternFunction() {
// 	eat(TOKEN_KEYWORD);

// 	if (!expectKeyword("function")) {
// 		printf("%s:%d:%d: " RED "error: " RESET "expected 'function' keyword, but got '%s'\n", 
// 					_filename, current->line, current->pos, current->data);
// 		return NULL;
// 	}
// 	next();

// 	Function *function = function_new();
// 	if (function == NULL) {
// 		log_error("unable to allocate memory for FunctionTemplate\n");
// 		return NULL;
// 	}

// 	function->is_extern = true;
// 	function->identifier = eatIdentifier();

// 	if (function->identifier == NULL) {
// 		function_free(function);
// 		return NULL;
// 	}

// 	if (!is(TOKEN_LPAREN)) {
// 		printf("%s:%d:%d: " RED "error: " RESET "expected '(', but got '%s'\n", _filename, current->line, current->pos, current->data);
// 		function_free(function);
// 		return NULL;
// 	}
// 	next();

// 	// parse argument types (if any are defined)

// 	if (!is(TOKEN_RPAREN)) {
// 		while (1) {
// 			if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENTIFIER)) {
// 				printf("%s:%d:%d: " RED "error: " RESET "expected type, but got '%s'\n", _filename, current->line, current->pos, current->data);
// 				function_free(function);
// 				return NULL;
// 			}

// 			// DatatypeOLD *type = parse_datatype(current->data);
// 			DataType *datatype = data_type_parse(current->data);
// 			if (datatype == NULL) {
// 				printf("%s:%d:%d: " RED "error: " RESET "unable to parse unknown type '%s'\n", _filename, current->line, current->pos, current->data);
// 				function_free(function);
// 				return NULL;
// 			}
// 			// TODO: check if type is an array type
// 			// FIXME: function->parameters is an array of Variable* not DataType*
// 			// arraylist_add(function->parameters, datatype);

// 			next();

// 			if (!is(TOKEN_COMMA)) {
// 				break;
// 			}

// 			next();
// 		}
// 	}

// 	eat(TOKEN_RPAREN);

// 	eat(TOKEN_COLON);

// 	// DatatypeOLD *return_typeOLD = parse_datatype(current->data);
// 	// if (return_typeOLD == NULL) {
// 	// 	printf("%s:%d:%d: " RED "error: " RESET "unable to parse unknown type '%s'\n", _filename, current->line, current->pos, current->data);
// 	// 	function_template_free(func_template);
// 	// 	return NULL;
// 	// }
// 	// // TODO: check if return type is an array type

// 	// func_template->return_typeOLD = return_typeOLD;

// 	func_template->return_type = strdup(current->data);
// 	next();

// 	eat(TOKEN_SEMICOLON);

// 	func_template->id = function_id_counter++;

// 	return func_template;
// }

Function *expectFunction() {
	if (!expectKeyword("function")) {	
		printf("%s:%d:%d: " RED "error: " RESET "expected function declaration\n", _filename, current->line, current->pos);
		return NULL;
	}
	next();

	Function *function = function_new();
	if (function == NULL) {
		log_error("unable to allocate memory for function\n");
		return NULL;
	}

	function->identifier = eatIdentifier();
	if (function->identifier == NULL) {
		function_free(function);
		log_error("calloc failed: unable to allocate memory for function identifier\n");
		return NULL;
	}

	// function paramerter
	if (!is(TOKEN_LPAREN)) {
		printf("%s:%d:%d: " RED "error: " RESET "expected '(', but got '%s'\n", _filename, current->line, current->pos, current->data);
		function_free(function);
		return NULL;
	}
	next();

	if (!is(TOKEN_RPAREN)) {
		while(1) {
			Variable *parameter = variable_new();
			
			if (!is(TOKEN_IDENTIFIER)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected parameter identifier, but got '%s'\n",
							_filename, current->line, current->pos, current->data);
				// free allocated memory
				function_free(function);
				variable_free(parameter);
				return NULL;
			}

			parameter->identifier = strdup(current->data);
			next();

			eat(TOKEN_COLON);

			if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENTIFIER)) {
				printf("%s:%d:%d: " RED "error: " RESET "expected type identifier, but got '%s'\n", _filename, current->line, current->pos, current->data);

				// free allocated memory
				function_free(function);
				variable_free(parameter);

				return NULL;
			}

			// parameter->type_identifier = strdup(current->data);
			parameter->type = data_type_parse(current->data);
			next();

			arraylist_add(function->parameters, parameter);

			if (!is(TOKEN_COMMA)) {
				break;
			}

			next();
		}
	}

	eat(TOKEN_RPAREN);
	eat(TOKEN_COLON);

	if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENTIFIER)) {
		printf("%s:%d:%d: " RED "error: " RESET "expected type identifier, but got '%s'\n", _filename, current->line, current->pos, current->data);
		function_free(function);
		return NULL;
	}

	function->return_type = data_type_parse(current->data);
	next();

	eat(TOKEN_LBRACE);


    while (!is(TOKEN_RBRACE)) {
        arraylist_add(function->statements, expectStatement());
    }

	function->id = function_id_counter++;

	eat(TOKEN_RBRACE);

	return function;
}

Variable *parseVariable() {

	Variable *variable = variable_new();
	if (variable == NULL) {
		log_error("unable to allocate memory for Variable\n");
		return NULL;
	}

	if (strcmp(current->data, "var") == 0) {
		variable->is_constant = 0;
	} else if (strcmp(current->data, "const") == 0) {
		variable->is_constant = 1;
	} else {
		log_error("unexpected token '%s' at [%d:%d], expected 'var' or 'const' keyword\n", current->data, current->line, current->pos);
		variable_free(variable);
		return NULL;
	}

	eat(TOKEN_KEYWORD); // 'var' keyword
	variable->identifier = eatIdentifier();
	if (variable->identifier == NULL) {
		log_error("unable to allocate memory for variable identifier\n");
		variable_free(variable);
		return NULL;
	}

	eat(TOKEN_COLON);

	// expect type identifier
	// this could be a keyword, aka. a primitive type
	// or an identifier, aka. a user-defined type
	if (!is(TOKEN_IDENTIFIER) && !is(TOKEN_KEYWORD)) {
		// primitive types are keywords
		printf("%s:%d:%d: " RED "error: " RESET "expected type identifier, but got '%s'\n", _filename, current->line, current->pos, current->data);
		variable_free(variable);
		return NULL;
	}

	// variable->type_identifier = strdup(current->data);
	variable->type = data_type_parse(current->data);
	next();

	int array_size_specified = 0;

	if (is(TOKEN_LBRACKET)) {
		eat(TOKEN_LBRACKET);
		// arraysize is specified
		if (is(TOKEN_NUMBER)) {
			variable->type->array_size = atoi(current->data);
			array_size_specified = 1;
			next();
		}
		eat(TOKEN_RBRACKET);
		variable->type->is_array = 1;
	}

	if (is(TOKEN_SEMICOLON)) {
		if (variable->is_constant) {
			printf("%s:%d:%d: " RED "error: " RESET "constant variables must be initialized\n", _filename, current->line, current->pos);
			variable_free(variable);
			return NULL;
		}

		if (variable->type->is_array) {
			printf("%s:%d:%d: " RED "error: " RESET "incomplete array declaration: either initialize array or specify array-size",
						_filename, current->line, current->pos);
			variable_free(variable);
			return NULL;
		}

		eat(TOKEN_SEMICOLON);

	} else if (is(TOKEN_ASSIGNMENT_SIMPLE)) {
		// default value assignment
		eat(TOKEN_ASSIGNMENT_SIMPLE);

		if (variable->type->is_array) {

			if (is(TOKEN_STRING)) {
				// SPECIAL CASE: strings
				variable->initializer = expression_new(EXPRESSION_TYPE_LITERAL);
				if (variable->initializer == NULL) {
					log_error("unable to allocate memory for Expression\n");
					variable_free(variable);
					return NULL;
				}
				
				variable->initializer->expr.literal = literal_create(LITERAL_STRING, current->data);
				if (variable->initializer->expr.literal == NULL) {
					log_error("unable to allocate memory for Literal\n");
					expression_free(variable->initializer);
					variable_free(variable);
					return NULL;
				}
				
				eat(TOKEN_STRING);
			} else {
				// normal array initialization
				eat(TOKEN_LBRACKET);

				variable->initializer = expectExpressionList();
				if (array_size_specified) {
					if (variable->initializer->expr.list->expressions->size != variable->type->array_size) {
						printf("%s:%d:%d: " RED "error: " RESET "array size mismatch: expected %lld elements, but got %ld",
									_filename, current->line, current->pos, variable->type->array_size,
									variable->initializer->expr.list->expressions->size);
						variable_free(variable);
						return NULL;
					}
				} else {
					variable->type->array_size = variable->initializer->expr.list->expressions->size;
				}

				eat(TOKEN_RBRACKET);
			}

		} else {
			Expression *initializer = expectExpression();
			variable->initializer = initializer;
		}
	}
	eat(TOKEN_SEMICOLON);

	return variable;
}

Enum *parseEnumDefinition() {
	if (!expectKeyword("enum")) {
		printf("%s:%d:%d: " RED "error: " RESET "expected 'enum' keyword\n", _filename, current->line, current->pos);
		return NULL;
	}
	next();

	Enum *enum_definition = enum_definition_new();
	if (enum_definition == NULL) {
		log_error("parseEnumDefinition(): cannot allocate memory for enum_definition\n");
		return NULL;
	}

	enum_definition->name = eatIdentifier();
	if (enum_definition->name == NULL) {
		enum_definition_free(enum_definition);
		return NULL;
	}

	eat(TOKEN_LBRACE);

	unsigned long long int index = 0;
	while (!is(TOKEN_RBRACE)) {

		EnumDefinitionMember *member = enum_definition_member_new(eatIdentifier(), index++);
		if (member == NULL) {
			enum_definition_free(enum_definition);
			return NULL;
		}

		arraylist_add(enum_definition->members, member);
		
		if (is(TOKEN_COMMA)) {
			eat(TOKEN_COMMA);
		} else {
			break;
		}
	}


	if (!is(TOKEN_RBRACE)) {
		printf("%s:%d:%d: " RED "error: " RESET "expected '}'\n", _filename, current->line, current->pos);
		enum_definition_free(enum_definition);
		return NULL;
	}

	next();

	return enum_definition;
}

Statement *expectStatement() {
    if (is(TOKEN_LBRACE)) {
        return expectCompoundStatement();
    } else if (is(TOKEN_KEYWORD)) {
		if (strcmp(current->data, "return") == 0) {
        	return expectJumpStatement();
		} else if (strcmp(current->data, "var") == 0 || strcmp(current->data, "const") == 0) {
			return expectVariableDeclarationStatement();
		} else if (strcmp(current->data, "if") == 0) {
			return expectConditionalStatement();
		} else if (strcmp(current->data, "while") == 0) {
			return expectLoopStatement();
		} else {
			log_error("unexpected keyword at [%d:%d]\n", current->line, current->pos);
			exit(1);
		}
    } else if (is(TOKEN_ASSEMBLY_CODE_BLOCK)) {
		AssemblyCodeBlockStatement *block = calloc(1, sizeof(AssemblyCodeBlockStatement));
		if (block == NULL) {
			log_error("expectStatement(): cannot allocate memory for block\n");
			return NULL;
		}
		block->code = calloc(strlen(current->data) + 1, sizeof(char));
		strcpy(block->code, current->data);

		eat(TOKEN_ASSEMBLY_CODE_BLOCK);

		Statement *statement = calloc(1, sizeof(Statement));
		if (statement == NULL) {
			log_error("expectStatement(): cannot allocate memory for statement\n");
			free(block);
			return NULL;
		}

		statement->type = STATEMENT_ASSEMBLY_CODE_BLOCK;
		statement->stmt.assembly = block;

		return statement;
	} else {
        return expectExpressionStatement();
    }
}

Statement *expectCompoundStatement() {
    expect(TOKEN_LBRACE);
    next();

    Statement *statement = calloc(1, sizeof(Statement));
    CompoundStatement *compund_statement = calloc(1, sizeof(CompoundStatement));

    ArrayList *statements_array = arraylist_create();

    while (!is(TOKEN_RBRACE)) {
        arraylist_add(statements_array, expectStatement());
    }

    expect(TOKEN_RBRACE);
    next();

    compund_statement->nested_statements = statements_array;
    statement->type = STATEMENT_COMPOUND;
    statement->stmt.compound = compund_statement;

    return statement;
}

Statement *expectExpressionStatement() {
    Statement *statement = calloc(1, sizeof(Statement));

	if (statement == NULL) {
		log_error("expectExpressionStatement(): calloc failed\n");
		exit(1);
	}

    statement->type = STATEMENT_EXPRESSION;

    ExpressionStatement *expr_stmt = calloc(1, sizeof(ExpressionStatement));
    if (expr_stmt == NULL) {
		free(statement);
		log_error("expectExpressionStatement(): calloc failed\n");
		exit(1);
	}
	
	Expression *expr = expectExpression();

    if (expr == NULL) {
        free(statement);
		free(expr_stmt);
        log_error("expectExpressionStatement(): expectExpression() returned NULL\n");
		exit(1);
    }

	expr_stmt->expression = expr;
    statement->stmt.expression = expr_stmt;

	eat(TOKEN_SEMICOLON);

    return statement;
}

Statement *expectJumpStatement() {
    expect(TOKEN_KEYWORD);

    Statement *statement = calloc(1, sizeof(Statement));

    if (strcmp(current->data, "return") == 0) {
        next();
        statement->type = STATEMENT_RETURN;
        ReturnStatement *ret_stmt = calloc(1, sizeof(ReturnStatement));
		if (is(TOKEN_SEMICOLON)) {
			ret_stmt->expression = NULL;
		} else {
			ret_stmt->expression = expectExpression();
		}
		statement->stmt._return = ret_stmt;
        eat(TOKEN_SEMICOLON);
    } else {
        free(statement);
        // error("unexpected keyword at [line:column]");
		log_error("unexpected keyword '%s' at [%d:%d]\n", current->data, current->line, current->pos);
		exit(1);
    }

    return statement;
}

Statement *expectVariableDeclarationStatement() {
	Statement *statement = calloc(1, sizeof(Statement));
	if (statement == NULL) {
		log_error("unable to allocate memory for Statement\n");
		return NULL;
	}

	VariableDeclarationStatement *var_decl_stmt = calloc(1, sizeof(VariableDeclarationStatement));
	if (var_decl_stmt == NULL) {
		free(statement);
		log_error("unable to allocate memory for VariableDeclarationStatement\n");
		return NULL;
	}

	statement->stmt.variable_declaration = var_decl_stmt;
	statement->type = STATEMENT_VARIABLE_DECLARATION;

	var_decl_stmt->variable = parseVariable();

	log_debug("expectVariableDeclarationStatement(): parsed variable %s\n", var_decl_stmt->variable->identifier);

	return statement;
}

Statement *expectConditionalStatement() {
	if (!strcmp(current->data, "if") == 0) {
		log_error("expectConditionalStatement(): unexpected token '%s'\n", current->data);
		return NULL;
	}

	eat(TOKEN_KEYWORD); // 'if'

	eat(TOKEN_LPAREN); // '('

	Expression *condition = expectExpression();
	if (condition == NULL) {
		log_error("expectConditionalStatement(): expectExpression() returned NULL\n");
		return NULL;
	}

	eat(TOKEN_RPAREN); // ')'

	Statement *body = expectStatement();
	if (body == NULL) {
		log_error("expectConditionalStatement(): expectStatement returned NULL\n");
		free(condition);
		return NULL;
	}

	Statement *statement = calloc(1, sizeof(Statement));
	if (statement == NULL) {
		log_error("expectConditionalStatement(): not able to allocated memory for Statement\n");
		free(condition);
		free(body);
		return NULL;
	}

	ConditionalStatement *conditional_statement = calloc(1, sizeof(ConditionalStatement));
	if (conditional_statement == NULL) {
		log_error("expectConditionalStatement(): not able to allocated memory for ConditionalStatement\n");
		free(condition);
		free(body);
		free(statement);
		return NULL;
	}

	statement->type = STATEMENT_CONDITIONAL;
	statement->stmt.conditional = conditional_statement;

	conditional_statement->condition = condition;
	conditional_statement->true_branch = body;
	conditional_statement->false_branch = NULL;

	if (is(TOKEN_KEYWORD) && strcmp(current->data, "else") == 0) {
		next();

		if (is(TOKEN_KEYWORD) && strcmp(current->data, "if") == 0) {
			Statement *else_conditional_statement = expectConditionalStatement();
			if (else_conditional_statement == NULL) {
				log_error("expectConditionalStatement(): expectConditionalStatement() failed\n");
				free(condition);
				free(body);
				free(statement);
				free(conditional_statement);
				return NULL;
			}
			conditional_statement->false_branch = else_conditional_statement;
		} else {
			conditional_statement->false_branch = expectStatement();
		}
	}

	return statement;
}

Statement *expectLoopStatement() {
	if (!strcmp(current->data, "while") == 0) {
		log_error("expectLoopStatement(): unexpected token '%s'\n", current->data);
		return NULL;
	}

	eat(TOKEN_KEYWORD); // 'while'

	eat(TOKEN_LPAREN); // '('

	Expression *condition = expectExpression();
	if (condition == NULL) {
		log_error("expectLoopStatement(): expectExpression() failed\n");
		return NULL;
	}

	eat(TOKEN_RPAREN); // ')'

	Statement *body = expectStatement();
	if (body == NULL) {
		log_error("expectLoopStatement(): expectStatement returned NULL\n");
		free(condition);
		return NULL;
	}

	Statement *statement = calloc(1, sizeof(Statement));
	if (statement == NULL) {
		log_error("expectLoopStatement(): not able to allocated memory for Statement\n");
		free(condition);
		free(body);
		return NULL;
	}

	LoopStatement *loop_statement = calloc(1, sizeof(LoopStatement));
	if (loop_statement == NULL) {
		log_error("expectLoopStatement(): not able to allocated memory for LoopStatement\n");
		free(condition);
		free(body);
		free(statement);
		return NULL;
	}

	statement->type = STATEMENT_LOOP;
	statement->stmt.loop = loop_statement;

	loop_statement->condition = condition;
	loop_statement->body = body;

	return statement;
}

// -----------------------------------------------------------------------------------

Expression *expectExpressionList() {
	ArrayList *expressions = arraylist_create();

	arraylist_add(expressions, expectExpression());

	while (is(TOKEN_COMMA)) {
		next();
		arraylist_add(expressions, expectExpression());
	}

	ExpressionList *expression_list = calloc(1, sizeof(ExpressionList));
	if (expression_list == NULL) {
		log_error("expectExpressionList(): not able to allocated memory for ExpressionList\n");
		arraylist_free(expressions);
		return NULL;
	}

	expression_list->expressions = expressions;

	Expression *expression = calloc(1, sizeof(Expression));
	if (expression == NULL) {
		log_error("expectExpressionList(): not able to allocated memory for Expression\n");
		arraylist_free(expressions);
		free(expression_list);
		return NULL;
	}

	expression->type = EXPRESSION_TYPE_LIST;
	expression->expr.list = expression_list;

	return expression;
}

Expression *expectExpression() {
	return expectAssignmentExpression();
    // return expectAdditiveExpression();
}

Expression *expectAssignmentExpression() {

	if (!isAssignmentOperator(lookahead)) {
		return expectLogicalOrExpression();
	}

	Expression *identifier_expr = expectUnaryExpression();

	if (!isAssignmentOperator(current)) {
		return identifier_expr;
	}

	switch (identifier_expr->type) {
		case EXPRESSION_TYPE_UNARY: // all unary expressions are valid (for now [lvc version 0.1.0-alpha])
			break;
		case EXPRESSION_TYPE_LITERAL:
			if (identifier_expr->expr.literal->type != LITERAL_IDENTIFIER) {
				log_error("expected identifier but got '%s' instead\n", LITERAL_TYPES[identifier_expr->expr.literal->type]);
				exit(1);
			}
			break;
		
		default:
			log_error("expected unary expression or identifier\n");
			exit(1);
	}

	Expression *expr = calloc(1, sizeof(Expression));
	AssignmentExpression *assignment_expr = calloc(1, sizeof(AssignmentExpression));

	if (expr == NULL || assignment_expr == NULL) {
		free(expr);
		free(assignment_expr);
		log_error("unable to allocate memory for assignment expression\n");
		return NULL;
	}

	expr->type = EXPRESSION_TYPE_ASSIGNMENT;
	expr->expr.assignment = assignment_expr;

	assignment_expr->identifier = identifier_expr;

	// assignment_expr->identifier = calloc(strlen(current->data) + 1, sizeof(char));
	// if (assignment_expr->identifier == NULL) {
	// 	free(expr);
	// 	free(assignment_expr->identifier);
	// 	free(assignment_expr);
	// 	log_error("unable to allocate memory for assignment expression\n");
	// 	return NULL;
	// }

	// strcpy(assignment_expr->identifier, current->data);
	// next();

	switch (current->type) {
		case TOKEN_ASSIGNMENT_SIMPLE: 
			assignment_expr->operator = ASSIGNMENT_OPERATOR_DEFAULT;
			break;
		case TOKEN_ASSIGNMENT_SUM:
			assignment_expr->operator = ASSIGNMENT_OPERATOR_ADD;
			break;
		case TOKEN_ASSIGNMENT_DIFFERENCE:
			assignment_expr->operator = ASSIGNMENT_OPERATOR_SUBTRACT;
			break;
		case TOKEN_ASSIGNMENT_PRODUCT:
			assignment_expr->operator = ASSIGNMENT_OPERATOR_MULTIPLY;
			break;
		case TOKEN_ASSIGNMENT_QUOTIENT:
			assignment_expr->operator = ASSIGNMENT_OPERATOR_DIVIDE;
			break;
		default:
			log_error("unknown assignment operator '%s' at [%d:%d]\n", current->data, current->line, current->pos);
			free(expr);
			free(assignment_expr->identifier);
			free(assignment_expr);
			return NULL;
	}

	next();

	assignment_expr->assignment_value = expectExpression();

	return expr;
}

// Expression *expectConditionalExpression() {}
Expression *expectLogicalOrExpression() {
	Expression *expr = expectLogicalAndExpression();

	if (!is(TOKEN_LOGICAL_OR)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr;

	while (is(TOKEN_LOGICAL_OR)) {
		temp->operator = BINARY_OPERATOR_LOGICAL_OR;
		next();
		temp->right = expectLogicalAndExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectLogicalAndExpression() {
	Expression *expr = expectBitwiseOrExpression();

	if (!is(TOKEN_LOGICAL_AND)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr;

	while (is(TOKEN_LOGICAL_AND)) {
		temp->operator = BINARY_OPERATOR_LOGICAL_AND;
		next();
		temp->right = expectBitwiseOrExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectBitwiseOrExpression() {
	Expression *expr = expectBitwiseXorExpression();

	if (!is(TOKEN_VERTICAL_BAR)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr;

	while (is(TOKEN_VERTICAL_BAR)) {
		temp->operator = BINARY_OPERATOR_BITWISE_OR;
		next();
		temp->right = expectBitwiseXorExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectBitwiseXorExpression() {
	Expression *expr = expectBitwiseAndExpression();

	if (!is(TOKEN_CIRCUMFLEX)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr;

	while (is(TOKEN_CIRCUMFLEX)) {
		temp->operator = BINARY_OPERATOR_BITWISE_XOR;
		next();
		temp->right = expectBitwiseAndExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectBitwiseAndExpression() {
	Expression *expr = expectEqualitiyExpression();

	if (!is(TOKEN_AMPERSAND)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr;

	while (is(TOKEN_AMPERSAND)) {
		temp->operator = BINARY_OPERATOR_BITWISE_AND;
		next();
		temp->right = expectEqualitiyExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectEqualitiyExpression() {
	Expression *expr = expectRelationalExpression();

	if (!is(TOKEN_RELATIONAL_EQUAL) && !is(TOKEN_RELATIONAL_NOT_EQUAL)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr = NULL;

	while (is(TOKEN_RELATIONAL_EQUAL) || is(TOKEN_RELATIONAL_NOT_EQUAL)) {
		temp->operator = is(TOKEN_RELATIONAL_EQUAL) ? BINARY_OPERATOR_LOGICAL_EQUAL : BINARY_OPERATOR_LOGICAL_NOT_EQUAL;
		next();
		temp->right = expectRelationalExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectRelationalExpression() {
	Expression *expr = expectShiftExpression();

	if (!is(TOKEN_RELATIONAL_GREATER) &&
				!is(TOKEN_RELATIONAL_GREATER_OR_EQUAL) &&
				!is(TOKEN_RELATIONAL_LESS) &&
				!is(TOKEN_RELATIONAL_LESS_OR_EQUAL)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr = NULL;

	while (is(TOKEN_RELATIONAL_GREATER) ||
				is(TOKEN_RELATIONAL_GREATER_OR_EQUAL) ||
				is(TOKEN_RELATIONAL_LESS) ||
				is(TOKEN_RELATIONAL_LESS_OR_EQUAL)) {
		switch (current->type) {
			case TOKEN_RELATIONAL_GREATER:
				temp->operator = BINARY_OPERATOR_LOGICAL_GREATER;
				break;
			case TOKEN_RELATIONAL_GREATER_OR_EQUAL:
				temp->operator = BINARY_OPERATOR_LOGICAL_GREATER_OR_EQUAL;
				break;
			case TOKEN_RELATIONAL_LESS:
				temp->operator = BINARY_OPERATOR_LOGICAL_LESS;
				break;
			case TOKEN_RELATIONAL_LESS_OR_EQUAL:
				temp->operator = BINARY_OPERATOR_LOGICAL_LESS_OR_EQUAL;
				break;
			default: // should never be reached
				break;
		}
		next();
		temp->right = expectShiftExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectShiftExpression() {
	Expression *expr = expectAdditiveExpression();

	if (!is(TOKEN_BITWISE_LEFT_SHIFT) && !is(TOKEN_BITWISE_RIGHT_SHIFT)) {
		return expr;
	}

	BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
	binary_expression->left = expr;
	BinaryExpression *temp = binary_expression;
	Expression *temp_expr = NULL;

	while (is(TOKEN_BITWISE_LEFT_SHIFT) || is(TOKEN_BITWISE_RIGHT_SHIFT)) {
		temp->operator = is(TOKEN_BITWISE_LEFT_SHIFT) ? BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT : BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT;
		next();
		temp->right = expectAdditiveExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression));
		temp_expr = calloc(1, sizeof(Expression));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary = binary_expression;
		temp->left = temp_expr;
	}

	free(temp_expr);

	Expression *expression = calloc(1, sizeof(Expression));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary = binary_expression;

	return expression;
}

Expression *expectAdditiveExpression() {
    Expression *expr = expectMultiplicativeExpression();

    if (!is(TOKEN_PLUS) && !is(TOKEN_MINUS)) {
        return expr;
    }

    BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
    binary_expression->left = expr;
    BinaryExpression *temp = binary_expression;
    Expression *temp_expr = NULL;

    while (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
        temp->operator = is(TOKEN_PLUS) ? BINARY_OPERATOR_ADD : BINARY_OPERATOR_SUBTRACT;
        next();
        temp->right = expectMultiplicativeExpression();
        binary_expression = temp;
        temp = calloc(1, sizeof(BinaryExpression));
        temp_expr = calloc(1, sizeof(Expression));
        temp_expr->type = EXPRESSION_TYPE_BINARY;
        temp_expr->expr.binary = binary_expression;
        temp->left = temp_expr;
    }

    free(temp_expr);

    Expression *expression = calloc(1, sizeof(Expression));
    expression->type = EXPRESSION_TYPE_BINARY;
    expression->expr.binary = binary_expression;

    return expression;
}

Expression *expectMultiplicativeExpression() {
    Expression *expr = expectUnaryExpression();

    if (!is(TOKEN_ASTERISK) && !is(TOKEN_SLASH) && !is(TOKEN_PERCENT)) {
        return expr;
    }

    BinaryExpression *binary_expression = calloc(1, sizeof(BinaryExpression));
    binary_expression->left = expr;
    BinaryExpression *temp = binary_expression;
    Expression *temp_expr = NULL;

    while (is(TOKEN_ASTERISK) || is(TOKEN_SLASH) || is(TOKEN_PERCENT)) {
        // temp->operator = is(TOKEN_ASTERISK) ? BINARY_OPERATOR_MULTIPLY : BINARY_OPERATOR_DIVIDE;
		if (is(TOKEN_ASTERISK)) {
			temp->operator = BINARY_OPERATOR_MULTIPLY;
		} else if (is(TOKEN_SLASH)) {
			temp->operator = BINARY_OPERATOR_DIVIDE;
		} else if (is(TOKEN_PERCENT)) {
			temp->operator = BINARY_OPERATOR_MODULO;
		} else {
			// this should never be reached
			log_error("Unexpected token type %d", current->type);
		}
        next();
        temp->right = expectUnaryExpression();
        binary_expression = temp;
        temp = calloc(1, sizeof(BinaryExpression));
        temp_expr = calloc(1, sizeof(Expression));
        temp_expr->type = EXPRESSION_TYPE_BINARY;
        temp_expr->expr.binary = binary_expression;
        temp->left = temp_expr;
    }

    free(temp_expr);

    Expression *expression = calloc(1, sizeof(Expression));
    expression->type = EXPRESSION_TYPE_BINARY;
    expression->expr.binary = binary_expression;

    return expression;
}

Expression *expectUnaryExpression() {
	switch (current->type) {
		case TOKEN_MINUS: {
			UnaryExpression *unary_expr = calloc(1, sizeof(UnaryExpression));
			if (unary_expr == NULL) {
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			unary_expr->operator = UNARY_OPERATOR_NEGATE;
			next();

			if (!is(TOKEN_NUMBER) && !is(TOKEN_IDENTIFIER)) {
				log_error("expectUnaryExpression(): Unexpected Token: expected number or identifier but got '%s' instead\n", TOKEN_TYPE_NAMES[current->type]);
				exit(1);
			}

			Literal *literal = calloc(1, sizeof(Literal));
			if (literal == NULL) {
				free(unary_expr);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			switch (current->type) {
				case TOKEN_IDENTIFIER: {
					literal->type = LITERAL_IDENTIFIER;
					literal->value = calloc(strlen(current->data), sizeof(char));
					strcpy(literal->value, current->data);
					next();
					break;
				}
				case TOKEN_NUMBER: {
					literal->type = LITERAL_NUMBER;
					literal->value = calloc(1 + strlen(current->data), sizeof(char));
					strcpy(literal->value, "-");
					strcat(literal->value, current->data);
					next();

					Expression *expr = calloc(1, sizeof(Expression));
					if (expr == NULL) {
						free(unary_expr);
						free(literal);
						log_error("expectUnaryExpression(): calloc failed\n");
						exit(1);
					}

					expr->type = EXPRESSION_TYPE_LITERAL;
					expr->expr.literal = literal;
					
					return expr;
				}
				default: {
					log_error("expectUnaryExpression(): an unexpected error occurred");
					log_error("please create an issue here: https://github.com/lucr4ft/luvascript-compiler/issues");
					exit(1);
				}
			}
			// literal->value = calloc(strlen(current->data), sizeof(char));
			// strcpy(literal->value, current->data);
			// next();

			unary_expr->identifier = literal;

			Expression *expr = calloc(1, sizeof(Expression));
			if (expr == NULL) {
				free(unary_expr);
				free(literal);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			expr->type = EXPRESSION_TYPE_UNARY;
			expr->expr.unary = unary_expr;

			return expr;
		} // case TOKEN_MINUS
		case TOKEN_INCREMENT: {
			UnaryExpression *unary_expr = calloc(1, sizeof(UnaryExpression));
			if (unary_expr == NULL) {
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			unary_expr->operator = UNARY_OPERATOR_INCREMENT;
			next();

			if (!is(TOKEN_IDENTIFIER)) {
				log_error("expectUnaryExpression(): Unexpected Token: expected identifier but got '%s' instead\n", TOKEN_TYPE_NAMES[current->type]);
				exit(1);
			}

			Literal *literal = calloc(1, sizeof(Literal));
			if (literal == NULL) {
				free(unary_expr);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			literal->type = LITERAL_IDENTIFIER;
			literal->value = calloc(strlen(current->data), sizeof(char));
			strcpy(literal->value, current->data);
			next();

			unary_expr->identifier = literal;

			Expression *expr = calloc(1, sizeof(Expression));
			if (expr == NULL) {
				free(unary_expr);
				free(literal);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			expr->type = EXPRESSION_TYPE_UNARY;
			expr->expr.unary = unary_expr;

			return expr;
		}

		default:
			return expectPostfixExpression();
	}
}

Expression *expectPostfixExpression() {
	switch (lookahead->type) {
		case TOKEN_LPAREN: {
			Expression *expr = calloc(1, sizeof(Expression));
			if (expr == NULL) {
				log_error("calloc failed: unable to allocate memory for Expression\n");
				return NULL;
			}

			FunctionCallExpression *func_call_expr = calloc(1, sizeof(FunctionCallExpression));
			if (func_call_expr == NULL) {
				free(expr);
				log_error("calloc failed: unable to allocate memory for FunctionCallExpression_T\n");
				return NULL;
			}

			expr->type = EXPRESSION_TYPE_FUNCTION_CALL;
			expr->expr.function_call = func_call_expr;

			func_call_expr->function_identifier = calloc(strlen(current->data), sizeof(char));
			strcpy(func_call_expr->function_identifier, current->data);
			next();

			eat(TOKEN_LPAREN);

			if (is(TOKEN_RPAREN)) {
				// no arguments specified
				func_call_expr->argument_expression_list = calloc(1, sizeof(ExpressionList));
				if (func_call_expr->argument_expression_list == NULL) {
					free(func_call_expr);
					free(expr);
					log_error("calloc failed: unable to allocate memory for ExpressionList\n");
					return NULL;
				}
				func_call_expr->argument_expression_list->expressions = arraylist_create();
			} else {
				// arguments specified
				Expression *argument_expr_list = expectExpressionList();
				if (argument_expr_list == NULL) {
					free(func_call_expr);
					free(expr);
					log_error("expectExpressionList() failed\n");
					return NULL;
				}
				func_call_expr->argument_expression_list = argument_expr_list->expr.list;
			}

			eat(TOKEN_RPAREN);

			return expr;
		}
		case TOKEN_LBRACKET: {
			Expression *expr = calloc(1, sizeof(Expression));
			if (expr == NULL) {
				log_error("calloc failed: unable to allocate memory for Expression\n");
				return NULL;
			}

			ArrayAccessExpression *array_access_expr = calloc(1, sizeof(ArrayAccessExpression));
			if (array_access_expr == NULL) {
				free(expr);
				log_error("calloc failed: unable to allocate memory for ArrayAccessExpression_T\n");
				return NULL;
			}

			expr->type = EXPRESSION_TYPE_ARRAY_ACCESS;
			expr->expr.array_access = array_access_expr;

			array_access_expr->identifier = calloc(1, sizeof(Literal));
			if (array_access_expr->identifier == NULL) {
				free(expr);
				free(array_access_expr);
				log_error("calloc failed: unable to allocate memory for Literal_T\n");
				return NULL;
			}

			array_access_expr->identifier->type = LITERAL_IDENTIFIER;
			array_access_expr->identifier->value = calloc(strlen(current->data), sizeof(char));
			strcpy(array_access_expr->identifier->value, current->data);
			next();

			eat(TOKEN_LBRACKET);

			Expression *index_expr = expectExpression();
			if (index_expr == NULL) {
				expression_free(expr);
				return NULL;
			}

			array_access_expr->index_expression = index_expr;

			eat(TOKEN_RBRACKET);

			return expr;
		}
		case TOKEN_DOT: {
			Expression *expr = calloc(1, sizeof(Expression));
			if (expr == NULL) {
				log_error("calloc failed: unable to allocate memory for Expression\n");
				return NULL;
			}

			MemberAccessExpression *member_access_expr = calloc(1, sizeof(MemberAccessExpression));
			if (member_access_expr == NULL) {
				free(expr);
				log_error("calloc failed: unable to allocate memory for MemberAccessExpression_T\n");
				return NULL;
			}

			expr->type = EXPRESSION_TYPE_MEMBER_ACCESS;
			expr->expr.member_access = member_access_expr;

			// FIXME: this would end in an infinite loop, bacause the lookahead
			// token will always be the dot
			// but it needs to expect a postfix expression (example: a[0].b)
			// a[0] is the identifer expression, and .b is the member access expression
			// but a[0] is a postfix expression
			// TODO: solve this without ending in an infinite loop
			// member_access_expr->identifier = expectPostfixExpression();
			member_access_expr->identifier = expectPrimaryExpression();

			eat(TOKEN_DOT);

			member_access_expr->member_identifier = eatIdentifier();

			return expr;
		}

		default:
			return expectPrimaryExpression();
	}
}

Expression *expectPrimaryExpression() {
	Expression *expression = calloc(1, sizeof(Expression));
	if (expression == NULL) {
		log_error("calloc failed: unable to allocate memory for Expression\n");
		return NULL;
	}

	if (is(TOKEN_LPAREN)) {
		eat(TOKEN_LPAREN);
		expression->type = EXPRESSION_TYPE_NESTED;
		Expression *nested_expression = expectExpression();
		if (nested_expression == NULL) {
			free(expression);
			log_error("expectPrimaryExpression(): calloc failed\n");
			return NULL;
		}

		// NestedExpression_T *nested_expression = calloc(1, sizeof(NestedExpression_T));
		// if (nested_expression == NULL) {
		// 	log_error("calloc failed: unable to allocate memory for NestedExpression_T\n");
		// 	free(expression);
		// 	free(expr);
		// 	return NULL;
		// }

		// nested_expression->expression = expr;
		expression->expr.nested = nested_expression;
		eat(TOKEN_RPAREN);
	} else if (is(TOKEN_IDENTIFIER) || is(TOKEN_NUMBER) || is(TOKEN_STRING) || is(TOKEN_KEYWORD) || is(TOKEN_CHAR)) {
		expression->type = EXPRESSION_TYPE_LITERAL;
		Literal *literal = calloc(1, sizeof(Literal));
		// free memory if calloc fails
		if (literal == NULL) {
			free(expression);
			log_error("calloc failed: unable to allocate memory for Literal_T\n");
			return NULL;
		}
		switch (current->type) {
			case TOKEN_IDENTIFIER: {
				literal->type = LITERAL_IDENTIFIER;
				literal->value = calloc(strlen(current->data), sizeof(char));
				strcpy(literal->value, current->data);
				break;
			}
			case TOKEN_STRING: {
				literal->type = LITERAL_STRING;
				literal->value = calloc(strlen(current->data), sizeof(char));
				strcpy(literal->value, current->data);
				break;
			}
			case TOKEN_NUMBER: {
				literal->type = LITERAL_NUMBER;
				literal->value = calloc(strlen(current->data), sizeof(char));
				strcpy(literal->value, current->data);
				break;
			}
			case TOKEN_KEYWORD: {
				if (strcmp(current->data, "true") == 0) {
					literal->type = LITERAL_BOOLEAN;
					literal->value = calloc(2, sizeof(char)); //"1"; // 1 == true
					literal->value[0] = '1';
				} else if (strcmp(current->data, "false") == 0) {
					literal->type = LITERAL_BOOLEAN;
					literal->value = calloc(2, sizeof(char));//"0"; // 0 = false
					literal->value[0] = '0';
				} else {
					log_error("unexpected keyword '%s' at [%d:%d]", current->data, current->line, current->pos);
					free(expression);
					free(literal);
					return NULL;
				}
				break;
			}
			case TOKEN_CHAR: {
				literal->type = LITERAL_CHARACTER;
				literal->value = calloc(strlen(current->data), sizeof(char));
				strcpy(literal->value, current->data);
				break;
			}
			default: {
				error("unknown error occured in parser.c#expectPrimaryExpression()");
			}
		}
		next();
		expression->expr.literal = literal;
	} else {
		log_error("expectPrimaryExpression(): expected nested expression or literal but got %s at [%d:%d]\n", TOKEN_TYPE_NAMES[current->type], current->line, current->pos);
		exit(1);
	}
	return expression;
}

char *eatIdentifier() {
	if (current->type != TOKEN_IDENTIFIER) {
		printf("%s:%d:%d: " RED "error: " RESET "expected identifier but got %s\n", _filename, current->line, current->pos, TOKEN_TYPE_NAMES[current->type]);
		return NULL;
	}

	char *identifier = strdup(current->data);
	if (identifier == NULL) {
		log_error("calloc failed: unable to allocate memory for identifier\n");
		return NULL;
	}
	
	next();
	
	return identifier;
}

bool expectKeyword(const char *keyword) {
	if (current->type != TOKEN_KEYWORD) {
		// log_error("expectKeyword(): expected keyword but got %s at [%d:%d]\n", TOKEN_TYPE_NAMES[current->type], current->line, current->pos);
		return false;
	}
	if (strcmp(current->data, keyword) != 0) {
		// log_error("expectKeyword(): expected keyword '%s' but got '%s' at [%d:%d]\n", keyword, current->data, current->line, current->pos);
		return false;
	}
	return true;
}
