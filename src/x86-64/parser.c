#include <arraylist.h>
#include <parser.h>
#include <x86-64/parser.h>
#include <statement.h>

char *convert_ast_to_x86_64_assembly(NODE *root_node) {
    return "";
}

char *parser_convert_function_to_assembly(Function *func) {
    char *code = "global ";

    stradd(code, func->name);
    stradd(code, "\n");
    stradd(code, func->name);
    stradd(code, ":\n");

    // TODO: load parameters

    for (int i = 0; i < func->statements->size; i++) {
        Statement *stmt = arraylist_get(func->statements, i);
        stradd(code, parser_convert_statement_to_assembly(stmt));
        stradd(code, "\n");
    }

    stradd(code, "ret\n");

    return code;
}

char *parser_convert_statement_to_assembly(Statement *stmt) {

}

char *parser_convert_expression_to_assembly(Expr* expr) {

}

char *mov(char *from, char *to);

char *mov(char *from, char *to) {
    char *template = "mov %s, %s";
    char *mov = calloc(sizeof(template) + sizeof(from) + sizeof(to), sizeof(char));
    sprintf(mov, template, from, to);
    return mov;
}