#include <types/analysis/analyser_impl.h>

#include <util/logging/logger.h>

bool analyse_statement(DataTypeTable *dtt, Statement *statement) {
    switch (statement->type) {
        case STATEMENT_EXPRESSION: {
            if (!analyse_expression(dtt, statement->stmt.expression->expression, statement->scope)) {
                return false;
            }

            return true;
        }
        case STATEMENT_RETURN: {
            if (!analyse_expression(dtt, statement->stmt._return->expression, statement->scope)) {
                return false;
            }

            // TODO: check if dt is compatible with the return type of the function

            return true;
        }

        case STATEMENT_VARIABLE_DECLARATION: {
            Variable *variable = statement->stmt.variable_declaration->variable;

            if (!analyse_variable(dtt, variable, statement->scope)) {
                return false;
            }

            return true;
        }
        case STATEMENT_CONDITIONAL: {
            ConditionalStatement *conditional = statement->stmt.conditional;
            
            // analyse condition
            if (!analyse_expression(dtt, conditional->condition, statement->scope)) {
                return false;
            }

            // analyse true branch
            if (!analyse_statement(dtt, conditional->true_branch)) {
                return false;
            }

            // analyse false branch (if it exists)
            if (conditional->false_branch != NULL) {
                if (!analyse_statement(dtt, conditional->false_branch)) {
                    return false;
                }
            }

            return true;
        }
        case STATEMENT_COMPOUND: {
            CompoundStatement *compound = statement->stmt.compound;
            
            for (size_t i = 0; i < arraylist_size(compound->nested_statements); i++) {
                Statement *nested_statement = arraylist_get(compound->nested_statements, i);
                if (!analyse_statement(dtt, nested_statement)) {
                    return false;
                }
            }

            return true;
        }

        default:
            return true; // FIXME: this is just a placeholder
    }

    return false;
}
