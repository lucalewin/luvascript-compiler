#include <token.h>
#include <types/literal.h>
#include <types/expression.h>
#include <types/statement.h>

const char *TOKEN_TYPE_NAMES[] = {
	[TOKEN_KEYWORD] = "token_keyword",
	[TOKEN_IDENTIFIER] = "token_identifier",
	[TOKEN_NUMBER] = "token_number",
	[TOKEN_STRING] = "token_string",
	[TOKEN_CHAR] = "token_char",
	[TOKEN_PLUS] = "token_plus",
	[TOKEN_MINUS] = "token_minus",
	[TOKEN_ASTERISK] = "token_asterisk",
	[TOKEN_SLASH] = "token_slash",
	[TOKEN_PERCENT] = "token_mod",
	[TOKEN_AMPERSAND] = "token_ampersand",
	[TOKEN_VERTICAL_BAR] = "token_vertical_bar",
	[TOKEN_POWER] = "token_power",
	[TOKEN_CIRCUMFLEX] = "token_circumflex",
	[TOKEN_COLON] = "token_colon",
	[TOKEN_SEMICOLON] = "token_semicolon",
	[TOKEN_DOT] = "token_dot",
	[TOKEN_COMMA] = "token_comma",
	[TOKEN_LPAREN] = "token_lparen",
	[TOKEN_RPAREN] = "token_rparen",
	[TOKEN_LBRACKET] = "token_lbracket",
	[TOKEN_RBRACKET] = "token_rbracket",
	[TOKEN_LBRACE] = "token_lbrace",
	[TOKEN_RBRACE] = "token_rbrace",
	[TOKEN_INCREMENT] = "token_increment",
	[TOKEN_DECREMENT] = "token_decrement",
	[TOKEN_BITWISE_LEFT_SHIFT] = "token_bitwise_left_shift",
	[TOKEN_BITWISE_RIGHT_SHIFT] = "token_bitwise_right_shift",
	[TOKEN_RELATIONAL_LESS] = "token_relational_less",
	[TOKEN_RELATIONAL_LESS_OR_EQUAL] = "token_relational_less_or_equal",
	[TOKEN_RELATIONAL_GREATER] = "token_relational_greather",
	[TOKEN_RELATIONAL_GREATER_OR_EQUAL] = "token_relational_greather_or_equal",
	[TOKEN_RELATIONAL_EQUAL] = "token_relation_equal",
	[TOKEN_RELATIONAL_NOT_EQUAL] = "token_relational_not_equal",
	[TOKEN_LOGICAL_AND] = "token_logical_and",
	[TOKEN_LOGICAL_OR] = "token_logical_or",
	[TOKEN_TILDE] = "token_tilde",
	[TOKEN_NOT] = "token_not",
	[TOKEN_QUESTION_MARK] = "token_questionmark",
	[TOKEN_ASSIGNMENT_SIMPLE] = "token_assignment_simple",
	[TOKEN_ASSIGNMENT_SUM] = "token_assignment_sum",
	[TOKEN_ASSIGNMENT_DIFFERENCE] = "token_assignment_difference",
	[TOKEN_ASSIGNMENT_PRODUCT] = "token_assignment_product",
	[TOKEN_ASSIGNMENT_QUOTIENT] = "token_assignment_quotient",
	[TOKEN_ASSIGNMENT_REMAINDER] = "token_assignment_remainder",
	[TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT] = "token_assignment_bitwise_left_shift",
	[TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT] = "token_assignment_bitwise_right_shift",
	[TOKEN_ASSIGNMENT_BITWISE_AND] = "token_assignment_bitwise_and",
	[TOKEN_ASSIGNMENT_BITWISE_XOR] = "token_assignment_bitwise_xor",
	[TOKEN_ASSIGNMENT_BITWISE_OR] = "token_assignment_bitwise_or",
	[TOKEN_ASSEMBLY_CODE_BLOCK] = "token_assembly_code_block",
};

const char *LITERAL_TYPES[] = {
	[LITERAL_IDENTIFIER] = "identifier_literal",
	[LITERAL_NUMBER] = "number_literal",
	[LITERAL_CHARACTER] = "char_literal",
	[LITERAL_STRING] = "string_literal",
	[LITERAL_BOOLEAN] = "boolean_literal"
};

const char *EXPRESSION_TYPES[] = {
	[EXPRESSION_TYPE_LITERAL] = "literal_expression",
	[EXPRESSION_TYPE_UNARY] = "unary_expression",
	[EXPRESSION_TYPE_BINARY] = "binary_expression",
	[EXPRESSION_TYPE_NESTED] = "nested_expression",
	[EXPRESSION_TYPE_FUNCTIONCALL] = "function_call_expression",
	[EXPRESSION_TYPE_ASSIGNMENT] = "assignment_expression",
	[EXPRESSION_TYPE_ARRAYACCESS] = "array_access_expression",
	[EXPRESSION_TYPE_LIST] = "list_expression"
};

const char* STATEMENT_TYPES[] = {
	[STATEMENT_COMPOUND] = "compound_statement",
	[STATEMENT_EXPRESSION] = "expression_statement",
	[STATEMENT_RETURN] = "return_statement",
	[STATEMENT_VARIABLE_DECLARATION] = "variable_declaration_statement",
	[STATEMENT_CONDITIONAL] = "conditional_statement",
	[STATEMENT_LOOP] = "loop_statement",
	[STATEMENT_ASSEMBLY_CODE_BLOCK] = "assembly_code_block_statement",
};