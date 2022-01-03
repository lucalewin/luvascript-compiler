#include <lexer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util.h>
#include <token.h>

#include <logging/logger.h>

#define keywords_length 19

char *keywords[keywords_length] = {
    "function",
    "var",
    "const",
    "return",
    "void",
    "true",
    "false",
    "null",
    "char",
    "string",
    "int",
    "long",
    "float",
    "double",
	"extern",
	"if",
	"else",
	"while",
	"asm",
};

ArrayList *tokenize(char *code) {
    ArrayList *list = arraylist_create();

    int line = 1, pos = 1;
    // move through code until end is reached
    while (*code != '\0') {
        if (*code == '\n') {
            line++; // increment line counter
            pos = 0; // reset char position counter
        }
        else if (is_whitespace(*code)); // continue cannot use continue keyword because *code is incremented at the end of the while-loop
        else if (is_number(*code)) {        // number
            // get length of number
            int i = 0;
            while (is_number(*(code + i++)));   // check if char is number, then increment pointer

            // copy the number to a char*
            char* number = malloc(sizeof(char) * i);
    	    substring(code, number, i-1);

            // create new token && add it to the list
            arraylist_add(list, token_create(number, TOKEN_NUMBER, line, pos));

            // move pointer by length of number
            //   2 needs to be subtracted because
            // i is incremented two times to much
            code = code + i - 2;
            pos += i - 2;
        } else if (*code == '"') {          // string literal
            // get length of string
            int i = 0;
            code++; // increment because *code is currently pointing to the first "
            while (*code != '\0' && *code != '"') {
                code++;
                i++;
                pos++;
            }

            // extract string from *code
            char *string = malloc(sizeof(char) * i);
            substring(code - i, string, i);

            // create new token && add it to the list
            arraylist_add(list, token_create(string, TOKEN_STRING, line, pos));
        } else if (*code == '\'') {
			code++; // increment because *code is currently pointing to the first '

			if (*(code + 1) != '\'') {
				log_error("unexpected character '%c' at [%d:%d], end of char literal\n", *code, line, pos);
			}

			char *character = calloc(2, sizeof(char));
			character[0] = *code;
			character[1] = '\0';

			code++; // increment because *code is currently pointing to the character

			// create new token and add it to the list
			arraylist_add(list, token_create(character, TOKEN_CHAR, line, pos));
		} else if ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {  // regex: [_a-zA-Z] -> identifier / keyword
            // get length of identifier
            int i = 0;
            while ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {
                code++;
                i++;
            }

            // extract identifier or keyword from *code
            char *identifier = malloc(sizeof(char) * i);
            substring(code - i, identifier, i);

            // check if identifier could be a keyword
            if (arr_contains(keywords, keywords_length, identifier)) {
                // create new token && add it to the list
				if (strcmp(identifier, "asm") != 0) {
                	arraylist_add(list, token_create(identifier, TOKEN_KEYWORD, line, pos));
				} else {
					// read assembly code block
					pos += i;
					while (*code != '\0' && is_whitespace(*code)) {
						code++;
						pos++;
					}
					if (*code != '{') {
						log_error("unexpected token '%c' at [%d:%d], expected '{'\n", *code, line, pos);
					}

					// get length of assembly code block
					int j = 0;
					code++; // increment because *code is currently pointing to the first '{'
					while (*code != '\0' && *code != '}') {
						j++;
						pos++;
						if (*code == '\n') {
							line++;
							pos = 0;
						}
						code++;
					}

					// extract assembly code block from *code
					char *assembly = malloc(sizeof(char) * j);
					substring(code - j, assembly, j);
					// log_debug("assembly code: %s\n", assembly);
					// free(assembly);
					code++;
					arraylist_add(list, token_create(assembly, TOKEN_ASSEMBLY_CODE_BLOCK, line, pos));
				}
            } else {
                // create new token && add it to the list
                arraylist_add(list, token_create(identifier, TOKEN_IDENTIFIER, line, pos));
            }

            pos += i;
            // decrement pointer because it was incremented one time to much
            code--;
        } else if (*code == '+') {
            switch (*(code + 1)) {
                case '+': {
                    arraylist_add(list, token_create(allocate_string("++"), TOKEN_INCREMENT, line, pos));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create(allocate_string("+="), TOKEN_ASSIGNMENT_SUM, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("+"), TOKEN_PLUS, line, pos));
                    break;
            }
        } else if (*code == '-') {
            switch (*(code + 1)) {
                case '-': {
                    arraylist_add(list, token_create(allocate_string("--"), TOKEN_DECREMENT, line, pos));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create(allocate_string("-="), TOKEN_ASSIGNMENT_DIFFERENCE, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("-"), TOKEN_MINUS, line, pos));
                    break;
            }
        } else if (*code == '*') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("*="), TOKEN_ASSIGNMENT_PRODUCT, line, pos));
                    code++;
                    break;
                }
                case '*': {
                    arraylist_add(list, token_create(allocate_string("**"), TOKEN_POWER, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("*"), TOKEN_ASTERISK, line, pos));
                    break;
            }
        } else if (*code == '/') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("/="), TOKEN_ASSIGNMENT_QUOTIENT, line, pos));
                    code++;
                    break;
                }
                case '/': {
                    while (*(++code) != '\n') {
                        // skip
                        line++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("/"), TOKEN_SLASH, line, pos));
                    break;
            }
        } else if (*code == '%') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("%="), TOKEN_ASSIGNMENT_REMAINDER, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("%"), TOKEN_PERCENT, line, pos));
                    break;
            }
        } else if (*code == '<') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("<="), TOKEN_RELATIONAL_LESS_OR_EQUAL, line, pos));
                    code++;
                    break;
                }
                case '<': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create(allocate_string("<<="), TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT, line, pos));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create(allocate_string("<<"), TOKEN_BITWISE_LEFT_SHIFT, line, pos));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("<"), TOKEN_RELATIONAL_LESS, line, pos));
                    break;
            }
        } else if (*code == '>') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string(">="), TOKEN_RELATIONAL_GREATER_OR_EQUAL, line, pos));
                    code++;
                    break;
                }
                case '>': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create(allocate_string(">>="), TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT, line, pos));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create(allocate_string(">>"), TOKEN_BITWISE_RIGHT_SHIFT, line, pos));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string(">"), TOKEN_RELATIONAL_GREATER, line, pos));
                    break;
            }
        } else if (*code == '!') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("!="), TOKEN_RELATIONAL_NOT_EQUAL, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("!"), TOKEN_NOT, line, pos));
                    break;
            }
        } else if (*code == '&') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("&="), TOKEN_ASSIGNMENT_BITWISE_AND, line, pos));
                    code++;
                    break;
                }
                case '&': {
                    arraylist_add(list, token_create(allocate_string("&&"), TOKEN_LOGICAL_AND, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("&"), TOKEN_AMPERSAND, line, pos));
                    break;
            }
        } else if (*code == '|') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("|="), TOKEN_ASSIGNMENT_BITWISE_OR, line, pos));
                    code++;
                    break;
                }
                case '|': {
                    arraylist_add(list, token_create(allocate_string("||"), TOKEN_LOGICAL_OR, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("|"), TOKEN_VERTICAL_BAR, line, pos));
                    break;
            }
        } else if (*code == '^') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(allocate_string("^="), TOKEN_ASSIGNMENT_BITWISE_XOR, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create(allocate_string("^"), TOKEN_CIRCUMFLEX, line, pos));
                    break;
            }
        } else if (*code == '=') {
			switch (*(code + 1)) {
				case '=': {
					arraylist_add(list, token_create(allocate_string("=="), TOKEN_RELATIONAL_EQUAL, line, pos));
					code++;
					break;
				}
				default:
					arraylist_add(list, token_create(allocate_string("="), TOKEN_ASSIGNMENT_SIMPLE, line, pos));
					break;
			}
        } else if (*code == '(') {
            arraylist_add(list, token_create(allocate_string("("), TOKEN_LPAREN, line, pos));
        } else if (*code == ')') {
            arraylist_add(list, token_create(allocate_string(")"), TOKEN_RPAREN, line, pos));
        } else if (*code == '[') {
            arraylist_add(list, token_create(allocate_string("["), TOKEN_LBRACKET, line, pos));
        } else if (*code == ']') {
            arraylist_add(list, token_create(allocate_string("]"), TOKEN_RBRACKET, line, pos));
        } else if (*code == '{') {
            arraylist_add(list, token_create(allocate_string("{"), TOKEN_LBRACE, line, pos));
        } else if (*code == '}') {
            arraylist_add(list, token_create(allocate_string("}"), TOKEN_RBRACE, line, pos));
        } else if (*code == ':') {
            arraylist_add(list, token_create(allocate_string(":"), TOKEN_COLON, line, pos));
        } else if (*code == ';') {
            arraylist_add(list, token_create(allocate_string(";"), TOKEN_SEMICOLON, line, pos));
        } else if (*code == '.') {
            arraylist_add(list, token_create(allocate_string("."), TOKEN_DOT, line, pos));
        } else if (*code == ',') {
            arraylist_add(list, token_create(allocate_string(","), TOKEN_COMMA, line, pos));
        } else if (*code == '~') {
            arraylist_add(list, token_create(allocate_string("~"), TOKEN_TILDE, line, pos));
        } else if (*code == '?') {
            arraylist_add(list, token_create(allocate_string("?"), TOKEN_QUESTION_MARK, line, pos));
        } else {
            printf("CHAR: %c\n", *code);
        }
        code++;
        pos++;
    }

    return list;
}
