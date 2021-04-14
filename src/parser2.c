#include <parser2.h>

ArrayList *tokens = NULL;
int index = 0;
Token *current;

void parse(AST *ast, ArrayList *list) {
    tokens = list;
    while (index < tokens->size) {
        Function *func = malloc(sizeof(Function));
        VariableDecl *var_decl = malloc(sizeof(VariableDecl));
        if (expect_function(func)) {
            arraylist_add(ast->functions, func);
            free(var_decl);
        } else if (expect_var_decl(var_decl)) {
            arraylist_add(ast->global_var_decls, var_decl);
            free(func);
        } else {
            printf("ERROR #0001: Expected Function or Variable declaration!\n");
            exit(1);
        }
    }
}

int expect_var_decl(VariableDecl *var_decl) {
    printf("expect VAR_DECL\n");
    current = arraylist_get(tokens, index++);
    if (expect_token(TOKEN_KEYWORD, current) && (strcmp(current->data, "var") == 0 || strcmp(current->data, "const") == 0)) {

        return 1;
    }
    return 0;
}

int expect_function(Function *func) {
    printf("expect FUNCTION\n");
    current = arraylist_get(tokens, index++);
    if (expect_token(TOKEN_KEYWORD, current) && strcmp(current->data, "function") == 0) {
        current = arraylist_get(tokens, index++);
        if (!expect_token(TOKEN_IDENDIFIER, current)) {
            printf("ERROR #0002: Expected function identifier at [%d,%d]\n", current->line, current->pos);
            exit(1);
        }
        func->name = malloc(strlen(current->data));
        strcpy(func->name, current->data);
        current = arraylist_get(tokens, index++);
        if (!expect_token(TOKEN_LPAREN, current)) {
            printf("ERROR #0003: Expected '(' at [%d,%d]\n", current->line, current->pos);
            exit(1);
        }
        FuncParam *param = malloc(sizeof(FuncParam));
        if (expect_func_param_decl(param)) {
            arraylist_add(func->params, param);
        }
        current = arraylist_get(tokens, index++);
        if (!expect_token(TOKEN_RPAREN, current)) {
            printf("ERROR #0004: Expected ')' at [%d,%d]\n", current->line, current->pos);
            exit(1);
        }
        current = arraylist_get(tokens, index++);
        if (expect_token(TOKEN_COLON, current)) {
            current = arraylist_get(tokens, index++);
            FuncReturnType *func_ret_type = malloc(sizeof(FuncReturnType));
            if (expect_func_return_type(func_ret_type)) {
                arraylist_add(func->return_types, func_ret_type);
            } else {
                printf ("ERROR #0007: Expected function return type delaration after ':' at [%d,%d]\n", current->line, current->pos);
                exit(1);
            }
        }
        current = arraylist_get(tokens, index++);
        if (!expect_token(TOKEN_LBRACE, current)) {
            printf("TYPE : %d\n", current->type);
            printf("ERROR #0005: Expected '{' at [%d,%d]\n", current->line, current->pos);
            exit(1);
        }
        current = arraylist_get(tokens, index++);
        while (!expect_token(TOKEN_RBRACE, current)) {
            index--;
            Statement *stmt = malloc(sizeof(Statement));
            if (expect_statement(stmt)) {
                arraylist_add(func->statements, stmt);
            } else {
                printf("ERROR #0006: Expected '}' at [%d,%d]\n", current->line, current->pos);
                exit(1);
            }
        }
        return 1;
    }
    return 0;
}

int expect_func_param_decl(FuncParam *func_param) {
    if (expect_token(TOKEN_IDENDIFIER, current)) {
        func_param->var_name = malloc(strlen(current->data));
        strcpy(func_param->var_name, current->data);
        return 1;
    }
    return 0;
}

int expect_func_return_type(FuncReturnType *func_ret_type) {
    printf("expect FUNCTION_RETURN_TYPE\n");
    DataType *type = malloc(sizeof(DataType));
    if (expect_datatype(type)) {
        func_ret_type->type = type;
        if (!expect_token(TOKEN_ASSIGNMENT_SIMPLE, arraylist_get(tokens, index+1))) {
            // next token is not '='
            // and no default value is defined
            // -> return FuncReturnType without default value
            return 1;
        }
        // parse expression
        current = arraylist_get(tokens, index += 2);
        Expr *expr = malloc(sizeof(Expr));
        if (!expect_expression(expr)) {
            // Todo: print line + position
            printf("ERROR #0008: Expected expression after '='!\n");
            exit(1);
        }
        func_ret_type->default_value = expr;
        return 1;
    }
    return 0;
}

int expect_statement(Statement *stmt) {
    printf("expect STATEMENT\n");
    BlockStatement *b_stmt = malloc(sizeof(BlockStatement));
    ReturnStatement *r_stmt = malloc(sizeof(ReturnStatement));
    ExprStatement *e_stmt = malloc(sizeof(ExprStatement));
    if (expect_block_statement(b_stmt)) {
        stmt->type = STATEMENT_BLOCK;
        stmt->statement = b_stmt;
        return 1;
    } else if (expect_return_statement(r_stmt)) {
        stmt->type = STATEMENT_RETURN;
        stmt->statement = r_stmt;
        return 1;
    } else if (expect_expression_statement(e_stmt)) {
        stmt->type = STATEMENT_EXPR;
        stmt->statement = e_stmt;
    } else {
        printf("ERROR #0009: Unexpected statement");
        return 0;
    }
}

int expect_block_statement(BlockStatement *b_stmt) {
    if (expect_token(TOKEN_LBRACE, current)) {
        current = arraylist_get(tokens, index++);
        while (!expect_token(TOKEN_RBRACE, current)) {
            
        }
        return 1;
    }
    return 0;
}

int expect_return_statement(ReturnStatement *r_stmt) {
    if (expect_token(TOKEN_KEYWORD, current) && strcmp(current->data, "return") == 0) {
        current = arraylist_get(tokens, index++);
        Expr *expr = malloc(sizeof(Expr));
        if (expect_expression(expr)) {
            r_stmt->expr = expr;
            return 1;
        }
        r_stmt->expr = NULL;
    }
    return 0;
}

int expect_expression_statement(ExprStatement *e_stmt) {
    Expr *expr = malloc(sizeof(Expr));
    if (expect_expression(expr)) {
        e_stmt->expr = expr;
        return 1;
    }
    return 0;
}

int expect_expression(Expr *expr) {
    return 0;
}

int expect_datatype(DataType *type) {
    printf("expect DATATYPE\n");
    if (expect_token(TOKEN_KEYWORD, current)) {
        type->type = TYPE_PRIMITIVE;
        type->type_name = malloc(strlen(current->data));
        strcpy(type->type_name, current->data);
        return 1;
    } else if (expect_token(TOKEN_IDENDIFIER, current)) {
        type->type = TYPE_STRUCT;
        type->type_name = malloc(strlen(current->data));
        strcpy(type->type_name, current->data);
        return 1;
    }
    return 0;
}

int expect_token(TokenType expected, Token *actual) {
    return expected == actual->type;
}
