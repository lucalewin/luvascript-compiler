#include <parser.h>
#include <x86-64/parser.h>

char *convert_ast_to_x86_64_assembly(NODE *root_node) {
    
    return "";
}

char *parser_convert_function_to_assembly(Function *func) {
    char *template = "global %s\n%s:\n";
    char *s = calloc(sizeof(template) + 2 * sizeof(func->name), sizeof(char));
    sprintf(s, template, func->name, func->name);
    printf("%s\n", s);
    return s;
}

char *parser_convert_statement_to_assembly(Statement *stmt) {

}

char *parser_convert_expression_to_assembly(Expr* expr) {

}

char *mov(char *from, char *to);
char *test(const char* format, ...);

char *mov(char *from, char *to) {
    char *template = "mov %s, %s";
    char *mov = calloc(sizeof(template) + sizeof(from) + sizeof(to), sizeof(char));
    sprintf(mov, template, from, to);
    return mov;
}

char *test(const char* format, ...) {
    
}