#include <types/analysis/analyser_impl.h>

bool analyse_statement(DataTypeTable *dtt, Statement *statement) {
    switch (statement->type) {
        case STATEMENT_EXPRESSION: {
            DataType * dt = analyse_expression(dtt, statement->stmt.expression_statement->expression, statement->scope);

            if (dt == NULL) {
                return false;
            }
            return true;
        }
        default:
            return true; // FIXME: this is just a placeholder
    }

    return false;
}
