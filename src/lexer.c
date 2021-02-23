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

TokenList *lexer_start(char *code) {
    // create new TokenList pointer (init)
    TokenList *list = tokenlist_init();

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
            tokenlist_add(list, token_create(number, TOKEN_NUMBER, 0, index));

            // move pointer by length of number
            // 2 needs to be subtracted because 
            // i is incremented two times to much
            code = &(*(code + i - 2));

            index += i - 2;
        } else if (*code == '"') {          // string literal
            // get length of string 
            int i = 0;
            *code++; // increment because *code is currently pointing to the first " 
            while (*code != '\0' && *code != '"') {
                *code++;
                i++;
                index++;
            }

            // extract string from *code
            char *string = malloc(sizeof(char) * i);
            substring(&(*(code - i)), string, i);

            // create new token && add it to the list
            tokenlist_add(list, token_create(string, TOKEN_STRING, 0, index));

        } else if ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {  // regex: [_a-zA-Z] -> identifier / keyword
            // get length of identifier 
            int i = 0;
            while ((*code >= 'A' && *code <= 'Z') || (*code >= 'a' && *code <= 'z') || *code == '_') {
                *code++;
                i++;
                index++;
            }

            // extract identifier or keyword from *code
            char *identifier = malloc(sizeof(char) * i);
            substring(&(*(code - i)), identifier, i);
            
            // check if identifier could be a keyword
            if (arr_contains(keywords, keywords_length, identifier)) {
                // create new token && add it to the list
                tokenlist_add(list, token_create(identifier, TOKEN_KEYWORD, 0, index));
            } else {
                // create new token && add it to the list
                tokenlist_add(list, token_create(identifier, TOKEN_IDENDIFIER, 0, index));
            }

            // decrement pointer because it was incremented one time to much
            *code--;
        } else if (*code == '+') {
            tokenlist_add(list, token_create("+", TOKEN_PLUS, 0, index));
        }else if (*code == '-') {
            tokenlist_add(list, token_create("-", TOKEN_MINUS, 0, index));
        }else if (*code == '*') {
            tokenlist_add(list, token_create("*", TOKEN_ASTERISK, 0, index));
        }else if (*code == '/') {
            tokenlist_add(list, token_create("/", TOKEN_SLASH, 0, index));
        }else if (*code == '%') {
            tokenlist_add(list, token_create("%", TOKEN_MOD, 0, index));
        } else if (*code == '(') {
            tokenlist_add(list, token_create("(", TOKEN_LPAREN, 0, index));
        } else if (*code == ')') {
            tokenlist_add(list, token_create(")", TOKEN_RPAREN, 0, index));
        } else if (*code == ':') {
            tokenlist_add(list, token_create(":", TOKEN_COLON, 0, index));
        } else if (*code == ';') {
            tokenlist_add(list, token_create(";", TOKEN_SEMICOLON, 0, index));
        } else if (*code == '.') {
            tokenlist_add(list, token_create(".", TOKEN_DOT, 0, index));
        } else {
            printf("CHAR: %c\n", *code);
        }
        *code++;
        index++;
    }
    // printf("\n\ntokens: %d\n", list->size);
    // for (int i = 0; i < list->size; i++) {
    //     printf("data: %s\n", list->tokens[i].data);
    // }
    return list;
}