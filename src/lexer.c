#include <lexer.h>

#include <stdio.h>

#include <util.h>
#include <token.h>

#define keywords_length 14

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
    "double"
};

ArrayList *tokenize(char *code) {
    ArrayList *list = arraylist_create();

    int index = 0;

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
            index += i - 2;
            pos += i - 2;
        } else if (*code == '"') {          // string literal
            // get length of string
            int i = 0;
            code++; // increment because *code is currently pointing to the first "
            while (*code != '\0' && *code != '"') {
                code++;
                i++;
                index++;
                pos++;
            }

            // extract string from *code
            char *string = malloc(sizeof(char) * i);
            substring(code - i, string, i);

            // create new token && add it to the list
            arraylist_add(list, token_create(string, TOKEN_STRING, line, pos));
        } else if ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {  // regex: [_a-zA-Z] -> identifier / keyword
            // get length of identifier
            int i = 0;
            while ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {
                code++;
                i++;
                index++;
            }

            // extract identifier or keyword from *code
            char *identifier = malloc(sizeof(char) * i);
            substring(code - i, identifier, i);

            // check if identifier could be a keyword
            if (arr_contains(keywords, keywords_length, identifier)) {
                // create new token && add it to the list
                arraylist_add(list, token_create(identifier, TOKEN_KEYWORD, line, pos));
            } else {
                // create new token && add it to the list
                arraylist_add(list, token_create(identifier, TOKEN_IDENDIFIER, line, pos));
            }

            pos += i;
            // decrement pointer because it was incremented one time to much
            code--;
        } else if (*code == '+') {
            switch (*(code + 1)) {
                case '+': {
                    arraylist_add(list, token_create("++", TOKEN_INCREMENT, line, pos));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create("+=", TOKEN_ASSIGNMENT_SUM, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("+", TOKEN_PLUS, line, pos));
                    break;
            }
        } else if (*code == '-') {
            switch (*(code + 1)) {
                case '-': {
                    arraylist_add(list, token_create("--", TOKEN_DECREMENT, line, pos));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create("-=", TOKEN_ASSIGNMENT_DIFFERENCE, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("-", TOKEN_MINUS, line, pos));
                    break;
            }
        } else if (*code == '*') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("*=", TOKEN_ASSIGNMENT_PRODUCT, line, pos));
                    code++;
                    break;
                }
                case '*': {
                    arraylist_add(list, token_create("**", TOKEN_POWER, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("*", TOKEN_ASTERISK, line, pos));
                    break;
            }
        } else if (*code == '/') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("/=", TOKEN_ASSIGNMENT_QUOTIENT, line, pos));
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
                    arraylist_add(list, token_create("/", TOKEN_SLASH, line, pos));
                    break;
            }
        } else if (*code == '%') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("%=", TOKEN_ASSIGNMENT_REMAINDER, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("%", TOKEN_MOD, line, pos));
                    break;
            }
        } else if (*code == '<') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("<=", TOKEN_RELATIONAL_LESS, line, pos));
                    code++;
                    break;
                }
                case '<': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create("<<=", TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT, line, pos));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create("<<", TOKEN_BITWISE_LEFT_SHIFT, line, pos));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create("<", TOKEN_RELATIONAL_LESS_OR_EQUAL, line, pos));
                    break;
            }
        } else if (*code == '>') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create(">=", TOKEN_RELATIONAL_GREATER, line, pos));
                    code++;
                    break;
                }
                case '>': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create(">>=", TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT, line, pos));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create(">>", TOKEN_BITWISE_RIGHT_SHIFT, line, pos));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create(">", TOKEN_RELATIONAL_GREATER_OR_EQUAL, line, pos));
                    break;
            }
        } else if (*code == '!') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("!=", TOKEN_RELATIONAL_NOT_EQUAL, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("!", TOKEN_NOT, line, pos));
                    break;
            }
        } else if (*code == '&') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("&=", TOKEN_ASSIGNMENT_BITWISE_AND, line, pos));
                    code++;
                    break;
                }
                case '&': {
                    arraylist_add(list, token_create("&&", TOKEN_LOGICAL_AND, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("&", TOKEN_AMPERSAND, line, pos));
                    break;
            }
        } else if (*code == '|') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("|=", TOKEN_ASSIGNMENT_BITWISE_OR, line, pos));
                    code++;
                    break;
                }
                case '|': {
                    arraylist_add(list, token_create("||", TOKEN_LOGICAL_OR, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("|", TOKEN_VERTICAL_BAR, line, pos));
                    break;
            }
        } else if (*code == '^') {
            switch (*(code + 1)) {
                case '=': {
                    arraylist_add(list, token_create("^=", TOKEN_ASSIGNMENT_BITWISE_XOR, line, pos));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("^", TOKEN_CIRCUMFLEX, line, pos));
                    break;
            }
        } else if (*code == '=') {
            arraylist_add(list, token_create("=", TOKEN_ASSIGNMENT_SIMPLE, line, pos));
        } else if (*code == '(') {
            arraylist_add(list, token_create("(", TOKEN_LPAREN, line, pos));
        } else if (*code == ')') {
            arraylist_add(list, token_create(")", TOKEN_RPAREN, line, pos));
        } else if (*code == '[') {
            arraylist_add(list, token_create("[", TOKEN_LBRACKET, line, pos));
        } else if (*code == ']') {
            arraylist_add(list, token_create("]", TOKEN_RBRACKET, line, pos));
        } else if (*code == '{') {
            arraylist_add(list, token_create("{", TOKEN_LBRACE, line, pos));
        } else if (*code == '}') {
            arraylist_add(list, token_create("}", TOKEN_RBRACE, line, pos));
        } else if (*code == ':') {
            arraylist_add(list, token_create(":", TOKEN_COLON, line, pos));
        } else if (*code == ';') {
            arraylist_add(list, token_create(";", TOKEN_SEMICOLON, line, pos));
        } else if (*code == '.') {
            arraylist_add(list, token_create(".", TOKEN_DOT, line, pos));
        } else if (*code == ',') {
            arraylist_add(list, token_create(",", TOKEN_COMMA, line, pos));
        } else if (*code == '~') {
            arraylist_add(list, token_create("~", TOKEN_TILDE, line, pos));
        } else if (*code == '?') {
            arraylist_add(list, token_create("?", TOKEN_QUESTION_MARK, line, pos));
        } else {
            printf("CHAR: %c\n", *code);
        }
        code++;
        index++;
        pos++;
    }

    return list;
}
