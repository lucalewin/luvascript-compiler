#include "include/lexer.h"

#define keywords_length 9

char *keywords[keywords_length] = {
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

ArrayList *lexer_start(char *code) {
    // create new TokenList pointer (init)
    // TokenList *list = tokenlist_init();

    ArrayList *list = arraylist_create();

    int index = 0;
    // move through code until end is reached
    while (*code != '\0') {
        if (is_whitespace(*code)); // continue cannot use continue keyword because *code is incremented at the end of the while-loop
        else if (is_number(*code)) {        // number
            // get length of number
            int i = 0;
            while (is_number(*(code + i++)));   // check if char is number, then increment pointer

            // copy the number to a char*
            char* number = malloc(sizeof(char) * i);
    	    substring(code, number, i-1);

            // create new token && add it to the list
            arraylist_add(list, token_create(number, TOKEN_NUMBER, 0, index));

            // move pointer by length of number
            //   2 needs to be subtracted because 
            // i is incremented two times to much
            code = code + i - 2;
            index += i - 2;
        } else if (*code == '"') {          // string literal
            // get length of string 
            int i = 0;
            code++; // increment because *code is currently pointing to the first " 
            while (*code != '\0' && *code != '"') {
                code++;
                i++;
                index++;
            }

            // extract string from *code
            char *string = malloc(sizeof(char) * i);
            substring(code - i, string, i);

            // create new token && add it to the list
            arraylist_add(list, token_create(string, TOKEN_STRING, 0, index));
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
                arraylist_add(list, token_create(identifier, TOKEN_KEYWORD, 0, index));
            } else {
                // create new token && add it to the list
                arraylist_add(list, token_create(identifier, TOKEN_IDENDIFIER, 0, index));
            }

            // decrement pointer because it was incremented one time to much
            code--;
        } else if (*code == '+') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '+': {
                    arraylist_add(list, token_create("++", TOKEN_INCREMENT, 0, index));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create("+=", TOKEN_ASSIGNMENT_SUM, 0, index));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("+", TOKEN_PLUS, 0, index));
                    break;
            }
        } else if (*code == '-') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '-': {
                    arraylist_add(list, token_create("--", TOKEN_DECREMENT, 0, index));
                    code++;
                    break;
                }
                case '=': {
                    arraylist_add(list, token_create("-=", TOKEN_ASSIGNMENT_DIFFERENCE, 0, index));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("-", TOKEN_MINUS, 0, index));
                    break;
            }
        } else if (*code == '*') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '=': {
                    arraylist_add(list, token_create("*=", TOKEN_ASSIGNMENT_PRODUCT, 0, index));
                    code++;
                    break;
                }
                case '*': {
                    arraylist_add(list, token_create("**", TOKEN_POWER, 0, index));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("*", TOKEN_ASTERISK, 0, index));
                    break;
            }
        } else if (*code == '/') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '=': {
                    arraylist_add(list, token_create("/=", TOKEN_ASSIGNMENT_QUOTIENT, 0, index));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("/", TOKEN_SLASH, 0, index));
                    break;
            }
        } else if (*code == '%') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '=': {
                    arraylist_add(list, token_create("%=", TOKEN_ASSIGNMENT_REMAINDER, 0, index));
                    code++;
                    break;
                }
                default:
                    arraylist_add(list, token_create("%", TOKEN_MOD, 0, index));
                    break;
            }
        } else if (*code == '<') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '=': {
                    arraylist_add(list, token_create("<=", TOKEN_RELATIONAL_LESS, 0, index));
                    code++;
                    break;
                }
                case '<': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create("<<=", TOKEN_ASSIGNMENT_BITWISE_LEFT_SHIFT, 0, index));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create("<<", TOKEN_BITWISE_LEFT_SHIFT, 0, index));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create("<", TOKEN_RELATIONAL_LESS_OR_EQUAL, 0, index));
                    break;
            }
        } else if (*code == '>') {
            switch (*(code + 1)) {
                case '\0':
                    break;
                case '=': {
                    arraylist_add(list, token_create(">=", TOKEN_RELATIONAL_GREATER, 0, index));
                    code++;
                    break;
                }
                case '>': {
                    if (*(code + 2) == '=') {
                        arraylist_add(list, token_create(">>=", TOKEN_ASSIGNMENT_BITWISE_RIGHT_SHIFT, 0, index));
                        code+=2;
                    } else {
                        arraylist_add(list, token_create(">>", TOKEN_BITWISE_RIGHT_SHIFT, 0, index));
                        code++;
                    }
                    break;
                }
                default:
                    arraylist_add(list, token_create(">", TOKEN_RELATIONAL_GREATER_OR_EQUAL, 0, index));
                    break;
            }
        } else if (*code == '(') {
            arraylist_add(list, token_create("(", TOKEN_LPAREN, 0, index));
        } else if (*code == ')') {
            arraylist_add(list, token_create(")", TOKEN_RPAREN, 0, index));
        } else if (*code == ':') {
            arraylist_add(list, token_create(":", TOKEN_COLON, 0, index));
        } else if (*code == ';') {
            arraylist_add(list, token_create(";", TOKEN_SEMICOLON, 0, index));
        } else if (*code == '.') {
            arraylist_add(list, token_create(".", TOKEN_DOT, 0, index));
        } else {
            printf("CHAR: %c\n", *code);
        }
        code++;
        index++;
    }

    // for (int i = 0; i < list->size; i++) {
    //     Token *t = arraylist_get(list, i);
    //     printf("TOKEN: %s\n", t->data);
    // }

    return list;
}