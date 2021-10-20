#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <logging/logger.h>

/**
 * 
 * global variables used by the expression parser
 * 
 */
int _index;
ArrayList *tokens;
Token *current;
Token *lookahead;

int expect(TokenType type) {
    if (current->type != type) {
        log_error("Expected token '%s' at [%d:%d]\n", TOKEN_TYPE_NAMES[type], current->line, current->pos);
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

AST *parse(ArrayList *token_list) {
    tokens = token_list;
    _index = 0;

    // load tokens into 'current' and 'lookahead'
    next();

    AST *root = calloc(1, sizeof(AST));

    Statement *stmt = expectStatement();
    if (stmt == NULL) {
        free(root);
		log_error("expected statement at [%d:%d]\n", current->line, current->pos);
		exit(1);
    }

    root->statement = stmt;

    return root;
}

Statement *expectStatement() {
    if (is(TOKEN_LBRACE)) {
        return expectCompoundStatement();
    } else if (is(TOKEN_KEYWORD)) {
		if (strcmp(current->data, "return") == 0) {
        	return expectJumpStatement();
		} else if (strcmp(current->data, "var") == 0) {
			return expectVariableDeclarationStatement();
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

    return statement;
}

Statement *expectJumpStatement() {
    expect(TOKEN_KEYWORD);

    Statement *statement = calloc(1, sizeof(Statement));

    if (strcmp(current->data, "return") == 0) {
        next();
        statement->type = STATEMENT_RETURN;
        ReturnStatement *ret_stmt = calloc(1, sizeof(ReturnStatement));
        ret_stmt->return_expression = expectExpression();
        statement->stmt.return_statement = ret_stmt;
        eat(TOKEN_SEMICOLON);
    } else {
        free(statement);
        error("unexpected keyword at [line:column]");
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

	Variable *variable = calloc(1, sizeof(Variable));
	if (variable == NULL) {
		free(statement);
		free(var_decl_stmt);
		log_error("unable to allocate memory for Variable\n");
		exit(1);
	}

	var_decl_stmt->var = variable;

	eat(TOKEN_KEYWORD); // 'var' keyword
	expect(TOKEN_IDENDIFIER); // the identifier of the variable
	// save identifier for later
	Literal_T *identifier = calloc(1, sizeof(Literal_T));
	if (identifier == NULL) {
		free(statement);
		free(var_decl_stmt);
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
		log_error("expectVariableDeclarationStatement(): expected type identifier at [%d:%d]", current->line, current->pos);
		exit(1);
	}

	if (strcmp(current->data, "int") == 0) {
		variable->datatype = 32;
	} else {
		log_error("expectVariableDeclarationStatement(): unexpected type identifier at [%d:%d]", current->line, current->pos);
		exit(1);
	}

	next();

	if (is(TOKEN_SEMICOLON)) {
		eat(TOKEN_SEMICOLON);
		Literal_T *default_value = calloc(1, sizeof(Literal_T));
		if (default_value == NULL) {
			free(statement);
			free(var_decl_stmt);
			free(variable);
			free(identifier);
			log_error("expectVariableDeclarationStatement(): cannot allocate memory for default_value\n");
			exit(1);
		}

		default_value->type = LITERAL_NUMBER;
		default_value->value = "0";

		Expression_T *default_value_expr = calloc(1, sizeof(Expression_T));
		if (default_value_expr == NULL) {
			free(statement);
			free(var_decl_stmt);
			free(variable);
			free(identifier);
			free(default_value);
			log_error("expectVariableDeclarationStatement(): cannot allocate memory for default_value_expr\n");
			exit(1);
		}

		default_value_expr->type = EXPRESSION_LITERAL;
		default_value_expr->expr.literal_expr = default_value;

		variable->default_value = default_value_expr;

		return statement;
	} else if (is(TOKEN_ASSIGNMENT_SIMPLE)) {
		// default value assignment
		eat(TOKEN_ASSIGNMENT_SIMPLE);

		Expression_T *default_value = expectExpression();
		variable->default_value = default_value;
	}

	eat(TOKEN_SEMICOLON);

	return statement;
}

// -----------------------------------------------------------------------------------

// expressions
// ArrayList *expectExpressionList() {}

Expression_T *expectExpression() {
    return expectAdditiveExpression();
}

// Expression_T *expectAssignmentExpression() {}
// Expression_T *expectConditionalExpression() {}
// Expression_T *expectLogicalOrExpression() {}
// Expression_T *expectLogicalAndExpression() {}
// Expression_T *expectBitwiseorExpression() {}
// Expression_T *expectBitwiseXorExpression() {}
// Expression_T *expectBitwiseAndExpression() {}
// Expression_T *expectEqualitiyExpression() {}
// Expression_T *expectRelationalExpression() {}
// Expression_T *expectShiftExpression() {}

Expression_T *expectAdditiveExpression() {
    Expression_T *expr = expectMultiplicativeExpression();

    if (!is(TOKEN_PLUS) && !is(TOKEN_MINUS)) {
        // log_debug("current token type: %s\n", current->data);
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
        temp_expr->type = EXPRESSION_BINARY;
        temp_expr->expr.binary_expr = binary_expression;
        temp->expression_left = temp_expr;
    }

    free(temp_expr);

    Expression_T *expression = calloc(1, sizeof(Expression_T));
    expression->type = EXPRESSION_BINARY;
    expression->expr.binary_expr = binary_expression;

    return expression;
}

Expression_T *expectMultiplicativeExpression() {
    Expression_T *expr = expectUnaryExpression();

    if (!is(TOKEN_ASTERISK) && !is(TOKEN_SLASH)) {
        // log_debug("current token type: %s\n", current->data);
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
        temp_expr->type = EXPRESSION_BINARY;
        temp_expr->expr.binary_expr = binary_expression;
        temp->expression_left = temp_expr;
    }

    free(temp_expr);

    Expression_T *expression = calloc(1, sizeof(Expression_T));
    expression->type = EXPRESSION_BINARY;
    expression->expr.binary_expr = binary_expression;

    return expression;
}

Expression_T *expectUnaryExpression() {
	switch (current->type) {
		case TOKEN_MINUS: {
			// log_debug("token minus\n");
			UnaryExpression_T *unary_expr = calloc(1, sizeof(UnaryExpression_T));
			if (unary_expr == NULL) {
				log_error("expectUnaryExpression(): calloc failed\n");
				exit(1);
			}

			unary_expr->operator = UNARY_OPERATOR_NEGATE;
			next();

			// log_debug("expectUnaryExpression(): current token type: %d | value: %s\n", current->type, current->data);

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

			expr->type = EXPRESSION_UNARY;
			expr->expr.unary_expr = unary_expr;

			return expr;
		}

		default:
			return expectPostfixExpression();
	}
}
Expression_T *expectPostfixExpression() {
    return expectPrimaryExpression();
}

Expression_T *expectPrimaryExpression() {
    Expression_T *expression = calloc(1, sizeof(Expression_T));
    if (is(TOKEN_LPAREN)) {
        eat(TOKEN_LPAREN);
        expression->type = EXPRESSION_NESTED;
        Expression_T *expr = expectExpression();
        NestedExpression_T *nested_expression = calloc(1, sizeof(NestedExpression_T));
        nested_expression->expression = expr;
        expression->expr.nested_expr = nested_expression;
        eat(TOKEN_RPAREN);
    } else if (is(TOKEN_IDENDIFIER) || is(TOKEN_NUMBER) || is(TOKEN_STRING)) {
        expression->type = EXPRESSION_LITERAL;
        Literal_T *literal = calloc(1, sizeof(Literal_T));
        switch (current->type) {
            case TOKEN_IDENDIFIER: {
                literal->type = LITERAL_IDENTIFIER;
                break;
            }
            case TOKEN_STRING: {
                literal->type = LITERAL_STRING;
                break;
            }
            case TOKEN_NUMBER: {
                literal->type = LITERAL_NUMBER;
                break;
            }
            default: {
                error("unknown error occured in parser.c#expectPrimaryExpression()");
            }
        }
        literal->value = calloc(strlen(current->data), sizeof(char));
        strcpy(literal->value, current->data);
        next();
        expression->expr.literal_expr = literal;
    } else {
        log_error("expectPrimaryExpression(): expected nested expression or literal but got %d\n", current);
        exit(1);
    }
    return expression;
}