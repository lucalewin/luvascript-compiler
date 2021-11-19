#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>

#include <scope_impl.h>

#include <datatypes.h>

// global variables used by the parser
int _index;
ArrayList *tokens;
Token *current;
Token *lookahead;

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

Variable *parseVariable();

// -----------------------------------------------------------------------------------

AST *parse(ArrayList *token_list) {
    tokens = token_list;
    _index = 0;

    // load tokens into 'current' and 'lookahead'
    next();

    AST *root = calloc(1, sizeof(AST));

	root->functions = arraylist_create();
	root->extern_functions = arraylist_create();
	root->global_variables = arraylist_create();

	while (_index < tokens->size) {
		if (strcmp(current->data, "function") == 0) {
			Function *func = expectFunction();
			arraylist_add(root->functions, func);
		} else if (strcmp(current->data, "var") == 0 || strcmp(current->data, "const") == 0) {
			// expect global variable/constant declaration
			
			Variable *glob_var = parseVariable();

			arraylist_add(root->global_variables, glob_var);

			// log_error("global variables are not supported yet\n");
			// exit(1);
		} else if (strcmp(current->data, "extern") == 0) {
			// expect extern function declaration
			FunctionTemplate *extern_func_template = expectExternFunctionTemplate();
			arraylist_add(root->extern_functions, extern_func_template);
		} else {
			log_error("unexpected token '%s' with value '%s' at [%d:%d]\n", TOKEN_TYPE_NAMES[current->type], current->data, current->line, current->pos);
			arraylist_free(root->functions);
			arraylist_free(root->extern_functions);
			arraylist_free(root->global_variables);
			free(root);
			// TODO: implement freeing of tokenlist in caller method
			// for (size_t i = token_list->size - 1; i >= 0; i--) {
			// 	Token *token = arraylist_get(token_list, i);
			// 	token_free(token);
			// }
			// arraylist_free(token_list);
			return NULL;
		}
	}

	if (!scope_evaluate_ast(root)) {
		ast_free(root);
	}

    return root;
}

FunctionTemplate *expectExternFunctionTemplate() {
	eat(TOKEN_KEYWORD);
	expect(TOKEN_KEYWORD); // 'function'
	if (strcmp(current->data, "function") != 0) {
		log_error("expected keyword 'function' at [%d:%d] but got '%s' with value '%s' instead\n", current->line, current->pos, TOKEN_TYPE_NAMES[current->type], current->data);
		exit(1);
	}
	next();

	// function identifier
	expect(TOKEN_IDENDIFIER);

	FunctionTemplate *func_template = calloc(1, sizeof(FunctionTemplate));
	if (func_template == NULL) {
		log_error("unable to allocate memory for FunctionTemplate\n");
		exit(1);
	}

	func_template->identifier = calloc(strlen(current->data), sizeof(char));
	if (func_template->identifier == NULL) {
		free(func_template);
		log_error("calloc failed: unable to allocate memory for function identifier\n");
		exit(1);
	}
    strcpy(func_template->identifier, current->data);
	next();

	eat(TOKEN_LPAREN);

	// parse argument types (if any are defined)

	func_template->param_datatypes = arraylist_create();

	if (!is(TOKEN_RPAREN)) {
		while (1) {
			if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
				log_error("expected type identifier at [%d:%d] but got %s instead\n", current->line, current->pos, TOKEN_TYPE_NAMES[current->type]);
			}

			Datatype *type = parse_datatype(current->data);
			arraylist_add(func_template->param_datatypes, type);

			next();

			if (!is(TOKEN_COMMA)) {
				break;
			}

			next();
		}
	}

	eat(TOKEN_RPAREN);

	eat(TOKEN_COLON);

	Datatype *return_type = parse_datatype(current->data);

	func_template->return_type = return_type;

	next();

	eat(TOKEN_SEMICOLON);

	return func_template;
}

Function *expectFunction() {
	if (!is(TOKEN_KEYWORD) && strcmp(current->data, "function") != 0) {	
		log_error("expected function declaration at [%d:%d] but got '%s' with value '%s' instead\n", current->line, current->pos, TOKEN_TYPE_NAMES[current->type], current->data);
		exit(1);
	}

	next();

	// expecting function name
	expect(TOKEN_IDENDIFIER);

	Function *function = calloc(1, sizeof(Function));
	if (function == NULL) {
		log_error("unable to allocate memory for function\n");
		exit(1);
	}

	function->identifier = calloc(strlen(current->data), sizeof(char));
	if (function->identifier == NULL) {
		free(function);
		log_error("calloc failed: unable to allocate memory for function identifier\n");
		exit(1);
	}
    strcpy(function->identifier, current->data);
	next();

	// function paramerter
	eat(TOKEN_LPAREN);

	function->parameters = arraylist_create();

	if (!is(TOKEN_RPAREN)) {
		while(1) {
			Variable *parameter = calloc(1, sizeof(Variable));
			
			if (!is(TOKEN_IDENDIFIER)) {
				expect(TOKEN_IDENDIFIER);
			}

			parameter->identifier = calloc(1, sizeof(Literal_T));
			if (parameter->identifier == NULL) {
				free(parameter);
				log_error("calloc failed: unable to allocate memory for parameter identifier\n");
				return NULL;
			}

			parameter->identifier->type = LITERAL_IDENTIFIER;
			parameter->identifier->value = calloc(strlen(current->data), sizeof(char));
			if (parameter->identifier->value == NULL) {
				free(parameter->identifier);
				free(parameter);
				log_error("calloc failed: unable to allocate memory for parameter identifier value\n");
				return NULL;
			}

			strcpy(parameter->identifier->value, current->data);
			next();

			expect(TOKEN_COLON); // `:`
			next();

			if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
				free(function->identifier);
				free(function);
				free(parameter->identifier->value);
				free(parameter);
				log_error("expected type identifier at [%d:%d] but got %s instead\n", current->line, current->pos, TOKEN_TYPE_NAMES[current->type]);
				exit(1);
			}

			parameter->datatype = parse_datatype(current->data);
			next();

			parameter->default_value = NULL;

			arraylist_add(function->parameters, parameter);

			if (!is(TOKEN_COMMA)) {
				break;
			}

			next();
		}
	}

	eat(TOKEN_RPAREN);

	// colon + return type
	eat(TOKEN_COLON);

	if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
		free(function->identifier);
		arraylist_free(function->parameters);
		free(function);
		log_error("expected type identifier at [%d:%d] but got %s instead\n", current->line, current->pos, TOKEN_TYPE_NAMES[current->type]);
		exit(1);
	}

	function->return_type = parse_datatype(current->data);
	next();

	eat(TOKEN_LBRACE);

	ArrayList *statements_array = arraylist_create();

    while (!is(TOKEN_RBRACE)) {
        arraylist_add(statements_array, expectStatement());
    }	

	function->body_statements = statements_array;

	eat(TOKEN_RBRACE);

	return function;
}

Variable *parseVariable() {

	Variable *variable = calloc(1, sizeof(Variable));
	if (variable == NULL) {
		log_error("unable to allocate memory for Variable\n");
		exit(1);
	}

	if (strcmp(current->data, "var") == 0) {
		variable->is_constant = 0;
	} else if (strcmp(current->data, "const") == 0) {
		variable->is_constant = 1;
	} else {
		log_error("unexpected token '%s' at [%d:%d], expected 'var' or 'const' keyword\n", current->data, current->line, current->pos);
		exit(1);
	}

	eat(TOKEN_KEYWORD); // 'var' keyword
	expect(TOKEN_IDENDIFIER); // the identifier of the variable
	// save identifier for later
	Literal_T *identifier = calloc(1, sizeof(Literal_T));
	if (identifier == NULL) {
		free(variable);
		log_error("unable to allocate memory for Literal_T\n");
		exit(1);
	}

	identifier->type = LITERAL_IDENTIFIER;
	identifier->value = calloc(strlen(current->data), sizeof(char));
	strcpy(identifier->value, current->data);

	variable->identifier = identifier;

	next();
	eat(TOKEN_COLON);

	// expect type identifier
	// this could be a keyword, aka. a primitive type
	// or an identifier
	if (!is(TOKEN_KEYWORD) && !is(TOKEN_IDENDIFIER)) {
		log_error("parseVariable(): expected type identifier at [%d:%d]\n", current->line, current->pos);
		free(variable);
		free(identifier->value);
		free(identifier);
		return NULL;
	}

	variable->datatype = parse_datatype(current->data);
	next();

	if (is(TOKEN_SEMICOLON)) {
		if (variable->is_constant) {
			log_error("parseVariable(): expected constant expression at [%d:%d]\n", current->line, current->pos);
			datatype_free(variable->datatype);
			free(variable);
			free(identifier->value);
			free(identifier);
			return NULL;
		}
		eat(TOKEN_SEMICOLON);
		Literal_T *default_value = calloc(1, sizeof(Literal_T));
		if (default_value == NULL) {
			datatype_free(variable->datatype);
			free(variable);
			free(identifier->value);
			free(identifier);
			log_error("parseVariable(): cannot allocate memory for default_value\n");
			return NULL;
		}

		default_value->type = LITERAL_NUMBER;
		default_value->value = "0";

		Expression_T *default_value_expr = calloc(1, sizeof(Expression_T));
		if (default_value_expr == NULL) {
			free(variable);
			free(identifier);
			free(default_value);
			log_error("parseVariable(): cannot allocate memory for default_value_expr\n");
			exit(1);
		}

		default_value_expr->type = EXPRESSION_TYPE_LITERAL;
		default_value_expr->expr.literal_expr = default_value;
		variable->default_value = default_value_expr;
	} else if (is(TOKEN_ASSIGNMENT_SIMPLE)) {
		// default value assignment
		eat(TOKEN_ASSIGNMENT_SIMPLE);

		Expression_T *default_value = expectExpression();
		variable->default_value = default_value;
	}
	eat(TOKEN_SEMICOLON);

	return variable;
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
    statement->stmt.compound_statement = compund_statement;

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
	
	Expression_T *expr = expectExpression();

    if (expr == NULL) {
        free(statement);
		free(expr_stmt);
        log_error("expectExpressionStatement(): expectExpression() returned NULL\n");
		exit(1);
    }

	expr_stmt->expression = expr;
    statement->stmt.expression_statement = expr_stmt;

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
        ret_stmt->expression = expectExpression();
        statement->stmt.return_statement = ret_stmt;
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
		exit(1);
	}

	VariableDeclarationStatement *var_decl_stmt = calloc(1, sizeof(VariableDeclarationStatement));
	if (var_decl_stmt == NULL) {
		free(statement);
		log_error("unable to allocate memory for VariableDeclarationStatement\n");
		exit(1);
	}

	statement->stmt.variable_decl = var_decl_stmt;
	statement->type = STATEMENT_VARIABLE_DECLARATION;

	var_decl_stmt->variable = parseVariable();

	return statement;
}

Statement *expectConditionalStatement() {
	if (!strcmp(current->data, "if") == 0) {
		log_error("expectConditionalStatement(): unexpected token '%s'\n", current->data);
		return NULL;
	}

	eat(TOKEN_KEYWORD); // 'if'

	eat(TOKEN_LPAREN); // '('

	Expression_T *condition = expectExpression();
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
	statement->stmt.conditional_statement = conditional_statement;

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

	Expression_T *condition = expectExpression();
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
	statement->stmt.loop_statement = loop_statement;

	loop_statement->condition = condition;
	loop_statement->body = body;

	return statement;
}

// -----------------------------------------------------------------------------------

ArrayList *expectExpressionList() {
	ArrayList *expressions = arraylist_create();

	arraylist_add(expressions, expectExpression());

	while (is(TOKEN_COMMA)) {
		next();
		arraylist_add(expressions, expectExpression());
	}

	return expressions;
}

Expression_T *expectExpression() {
	return expectAssignmentExpression();
    // return expectAdditiveExpression();
}

Expression_T *expectAssignmentExpression() {

	if (!isAssignmentOperator(lookahead)) {
		return expectLogicalOrExpression();
	}

	Expression_T *identifier_expr = expectUnaryExpression();

	if (!isAssignmentOperator(current)) {
		return identifier_expr;
	}

	switch (identifier_expr->type) {
		case EXPRESSION_TYPE_UNARY: // all unary expressions are valid (for now [lvc version 0.1.0-alpha])
			break;
		case EXPRESSION_TYPE_LITERAL:
			if (identifier_expr->expr.literal_expr->type != LITERAL_IDENTIFIER) {
				log_error("expected identifier but got '%s' instead\n", LITERAL_TYPES[identifier_expr->expr.literal_expr->type]);
				exit(1);
			}
			break;
		
		default:
			log_error("expected unary expression or identifier\n");
			exit(1);
	}

	Expression_T *expr = calloc(1, sizeof(Expression_T));
	AssignmentExpression_T *assignment_expr = calloc(1, sizeof(AssignmentExpression_T));

	if (expr == NULL || assignment_expr == NULL) {
		free(expr);
		free(assignment_expr);
		log_error("unable to allocate memory for assignment expression\n");
		return NULL;
	}

	expr->type = EXPRESSION_TYPE_ASSIGNMENT;
	expr->expr.assignment_expr = assignment_expr;

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

// Expression_T *expectConditionalExpression() {}
Expression_T *expectLogicalOrExpression() {
	Expression_T *expr = expectLogicalAndExpression();

	if (!is(TOKEN_LOGICAL_OR)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_LOGICAL_OR)) {
		temp->operator = BINARY_OPERATOR_LOGICAL_OR;
		next();
		temp->expression_right = expectLogicalAndExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectLogicalAndExpression() {
	Expression_T *expr = expectBitwiseOrExpression();

	if (!is(TOKEN_LOGICAL_AND)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_LOGICAL_AND)) {
		temp->operator = BINARY_OPERATOR_LOGICAL_AND;
		next();
		temp->expression_right = expectBitwiseOrExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectBitwiseOrExpression() {
	Expression_T *expr = expectBitwiseXorExpression();

	if (!is(TOKEN_VERTICAL_BAR)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_VERTICAL_BAR)) {
		temp->operator = BINARY_OPERATOR_BITWISE_OR;
		next();
		temp->expression_right = expectBitwiseXorExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectBitwiseXorExpression() {
	Expression_T *expr = expectBitwiseAndExpression();

	if (!is(TOKEN_CIRCUMFLEX)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_CIRCUMFLEX)) {
		temp->operator = BINARY_OPERATOR_BITWISE_XOR;
		next();
		temp->expression_right = expectBitwiseAndExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectBitwiseAndExpression() {
	Expression_T *expr = expectEqualitiyExpression();

	if (!is(TOKEN_AMPERSAND)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_AMPERSAND)) {
		temp->operator = BINARY_OPERATOR_BITWISE_AND;
		next();
		temp->expression_right = expectEqualitiyExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectEqualitiyExpression() {
	Expression_T *expr = expectRelationalExpression();

	if (!is(TOKEN_RELATIONAL_EQUAL) && !is(TOKEN_RELATIONAL_NOT_EQUAL)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_RELATIONAL_EQUAL) || is(TOKEN_RELATIONAL_NOT_EQUAL)) {
		temp->operator = is(TOKEN_RELATIONAL_EQUAL) ? BINARY_OPERATOR_LOGICAL_EQUAL : BINARY_OPERATOR_LOGICAL_NOT_EQUAL;
		next();
		temp->expression_right = expectRelationalExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectRelationalExpression() {
	Expression_T *expr = expectShiftExpression();

	if (!is(TOKEN_RELATIONAL_GREATER) &&
				!is(TOKEN_RELATIONAL_GREATER_OR_EQUAL) &&
				!is(TOKEN_RELATIONAL_LESS) &&
				!is(TOKEN_RELATIONAL_LESS_OR_EQUAL)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_RELATIONAL_GREATER) ||
				is(TOKEN_RELATIONAL_GREATER_OR_EQUAL) ||
				is(TOKEN_RELATIONAL_LESS) ||
				is(TOKEN_RELATIONAL_LESS_OR_EQUAL)) {
		switch (current->type) {
			case TOKEN_RELATIONAL_GREATER:
				temp->operator = BINARY_OPERATOR_LOGICAL_GREATHER;
				break;
			case TOKEN_RELATIONAL_GREATER_OR_EQUAL:
				temp->operator = BINARY_OPERATOR_LOGICAL_GREATHER_OR_EQUAL;
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
		temp->expression_right = expectShiftExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectShiftExpression() {
	Expression_T *expr = expectAdditiveExpression();

	if (!is(TOKEN_BITWISE_LEFT_SHIFT) && !is(TOKEN_BITWISE_RIGHT_SHIFT)) {
		return expr;
	}

	BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
	binary_expression->expression_left = expr;
	BinaryExpression_T *temp = binary_expression;
	Expression_T *temp_expr;

	while (is(TOKEN_BITWISE_LEFT_SHIFT) || is(TOKEN_BITWISE_RIGHT_SHIFT)) {
		temp->operator = is(TOKEN_BITWISE_LEFT_SHIFT) ? BINARY_OPERATOR_BITWISE_ARITHMETIC_LEFT_SHIFT : BINARY_OPERATOR_BITWISE_ARITHMETIC_RIGHT_SHIFT;
		next();
		temp->expression_right = expectAdditiveExpression();
		binary_expression = temp;
		temp = calloc(1, sizeof(BinaryExpression_T));
		temp_expr = calloc(1, sizeof(Expression_T));
		temp_expr->type = EXPRESSION_TYPE_BINARY;
		temp_expr->expr.binary_expr = binary_expression;
		temp->expression_left = temp_expr;
	}

	free(temp_expr);

	Expression_T *expression = calloc(1, sizeof(Expression_T));
	expression->type = EXPRESSION_TYPE_BINARY;
	expression->expr.binary_expr = binary_expression;

	return expression;
}

Expression_T *expectAdditiveExpression() {
    Expression_T *expr = expectMultiplicativeExpression();

    if (!is(TOKEN_PLUS) && !is(TOKEN_MINUS)) {
        return expr;
    }

    BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
    binary_expression->expression_left = expr;
    BinaryExpression_T *temp = binary_expression;
    Expression_T *temp_expr;

    while (is(TOKEN_PLUS) || is(TOKEN_MINUS)) {
        temp->operator = is(TOKEN_PLUS) ? BINARY_OPERATOR_PLUS : BINARY_OPERATOR_MINUS;
        next();
        temp->expression_right = expectMultiplicativeExpression();
        binary_expression = temp;
        temp = calloc(1, sizeof(BinaryExpression_T));
        temp_expr = calloc(1, sizeof(Expression_T));
        temp_expr->type = EXPRESSION_TYPE_BINARY;
        temp_expr->expr.binary_expr = binary_expression;
        temp->expression_left = temp_expr;
    }

    free(temp_expr);

    Expression_T *expression = calloc(1, sizeof(Expression_T));
    expression->type = EXPRESSION_TYPE_BINARY;
    expression->expr.binary_expr = binary_expression;

    return expression;
}

Expression_T *expectMultiplicativeExpression() {
    Expression_T *expr = expectUnaryExpression();

    if (!is(TOKEN_ASTERISK) && !is(TOKEN_SLASH)) {
        return expr;
    }

    BinaryExpression_T *binary_expression = calloc(1, sizeof(BinaryExpression_T));
    binary_expression->expression_left = expr;
    BinaryExpression_T *temp = binary_expression;
    Expression_T *temp_expr;

    while (is(TOKEN_ASTERISK) || is(TOKEN_SLASH)) {
        temp->operator = is(TOKEN_ASTERISK) ? BINARY_OPERATOR_MULTIPLY : BINARY_OPERATOR_DIVIDE;
        next();
        temp->expression_right = expectUnaryExpression();
        binary_expression = temp;
        temp = calloc(1, sizeof(BinaryExpression_T));
        temp_expr = calloc(1, sizeof(Expression_T));
        temp_expr->type = EXPRESSION_TYPE_BINARY;
        temp_expr->expr.binary_expr = binary_expression;
        temp->expression_left = temp_expr;
    }

    free(temp_expr);

    Expression_T *expression = calloc(1, sizeof(Expression_T));
    expression->type = EXPRESSION_TYPE_BINARY;
    expression->expr.binary_expr = binary_expression;

    return expression;
}

Expression_T *expectUnaryExpression() {
	switch (current->type) {
		case TOKEN_MINUS: {
			UnaryExpression_T *unary_expr = calloc(1, sizeof(UnaryExpression_T));
			if (unary_expr == NULL) {
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			unary_expr->operator = UNARY_OPERATOR_NEGATE;
			next();

			if (!is(TOKEN_NUMBER) && !is(TOKEN_IDENDIFIER)) {
				log_error("expectUnaryExpression(): Unexpected Token: expected number or identifier but got '%s' instead\n", TOKEN_TYPE_NAMES[current->type]);
				exit(1);
			}

			Literal_T *literal = calloc(1, sizeof(Literal_T));
			if (literal == NULL) {
				free(unary_expr);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			switch (current->type) {
				case TOKEN_IDENDIFIER: {
					literal->type = LITERAL_IDENTIFIER;
					break;
				}
				case TOKEN_NUMBER: {
					literal->type = LITERAL_NUMBER;
					break;
				}
				default: {
					log_error("expectUnaryExpression(): an unexpected error occurred");
					log_error("please create an issue here: https://github.com/lucr4ft/luvascript-compiler/issues");
					exit(1);
				}
			}
			literal->value = calloc(strlen(current->data), sizeof(char));
			strcpy(literal->value, current->data);
			next();

			unary_expr->identifier = literal;

			Expression_T *expr = calloc(1, sizeof(Expression_T));
			if (expr == NULL) {
				free(unary_expr);
				free(literal);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			expr->type = EXPRESSION_TYPE_UNARY;
			expr->expr.unary_expr = unary_expr;

			return expr;
		} // case TOKEN_MINUS
		case TOKEN_INCREMENT: {
			UnaryExpression_T *unary_expr = calloc(1, sizeof(UnaryExpression_T));
			if (unary_expr == NULL) {
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			unary_expr->operator = UNARY_OPERATOR_INCREMENT;
			next();

			if (!is(TOKEN_IDENDIFIER)) {
				log_error("expectUnaryExpression(): Unexpected Token: expected identifier but got '%s' instead\n", TOKEN_TYPE_NAMES[current->type]);
				exit(1);
			}

			Literal_T *literal = calloc(1, sizeof(Literal_T));
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

			Expression_T *expr = calloc(1, sizeof(Expression_T));
			if (expr == NULL) {
				free(unary_expr);
				free(literal);
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			expr->type = EXPRESSION_TYPE_UNARY;
			expr->expr.unary_expr = unary_expr;

			return expr;
		}

		default:
			return expectPostfixExpression();
	}
}

Expression_T *expectPostfixExpression() {
	if (!is(TOKEN_IDENDIFIER) || lookahead->type != TOKEN_LPAREN) {
		return expectPrimaryExpression();
	}

	Expression_T *expr = calloc(1, sizeof(Expression_T));
	if (expr == NULL) {
		log_error("calloc failed: unable to allocate memory for Expression_T\n");
		exit(1);
	}

	FunctionCallExpression_T *func_call_expr = calloc(1, sizeof(FunctionCallExpression_T));
	if (func_call_expr == NULL) {
		free(expr);
		log_error("calloc failed: unable to allocate memory for FunctionCallExpression_T\n");
		exit(1);
	}

	expr->type = EXPRESSION_TYPE_FUNCTIONCALL;
	expr->expr.func_call_expr = func_call_expr;

	func_call_expr->function_identifier = calloc(strlen(current->data), sizeof(char));
	strcpy(func_call_expr->function_identifier, current->data);
	next();

	eat(TOKEN_LPAREN);

	if (is(TOKEN_RPAREN)) {
		// no arguments specified
		func_call_expr->argument_expression_list = arraylist_create();
	} else {
		// arguments specified
		func_call_expr->argument_expression_list = expectExpressionList();
	}

	eat(TOKEN_RPAREN);

	return expr;
}

Expression_T *expectPrimaryExpression() {
    Expression_T *expression = calloc(1, sizeof(Expression_T));
    if (is(TOKEN_LPAREN)) {
        eat(TOKEN_LPAREN);
        expression->type = EXPRESSION_TYPE_NESTED;
        Expression_T *expr = expectExpression();
        NestedExpression_T *nested_expression = calloc(1, sizeof(NestedExpression_T));
        nested_expression->expression = expr;
        expression->expr.nested_expr = nested_expression;
        eat(TOKEN_RPAREN);
    } else if (is(TOKEN_IDENDIFIER) || is(TOKEN_NUMBER) || is(TOKEN_STRING) || is(TOKEN_KEYWORD)) {
        expression->type = EXPRESSION_TYPE_LITERAL;
        Literal_T *literal = calloc(1, sizeof(Literal_T));
        switch (current->type) {
            case TOKEN_IDENDIFIER: {
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
            default: {
                error("unknown error occured in parser.c#expectPrimaryExpression()");
            }
        }
        next();
        expression->expr.literal_expr = literal;
    } else {
        log_error("expectPrimaryExpression(): expected nested expression or literal but got %s at [%d:%d]\n", TOKEN_TYPE_NAMES[current->type], current->line, current->pos);
        exit(1);
    }
    return expression;
}