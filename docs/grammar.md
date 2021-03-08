# Luva Language Specification v0.1

```bnf
<program>               ::= <function> | <program> <function>

<function>              ::= 'function' <identifier> '(' [<parameter>] ')' ':' [<function return types>] '{' [<statement list>] '}'

<function return types> ::= <identifier> [',' <function return types>]
                          | <identifier> ':' <type> [',' <function return types>]
                          | <identifier> ':' <type> '=' <expression> [',' <function return types>]
                          | <identifier> '=' <expression> [',' <function return types>]

# ---------------- [ Statements ] ----------------

<statement list>        ::= <statement> | <statement list> <statement>

<statement>             ::= <block statement>
                          | <variable declaration>
                          | <return statement>
                          | <no-op statement>

<block statement>       ::= '{' <statement> '}'
<variable declaration>  ::= ('var' | 'const') <identifier> '=' <expression> (';' | ',' <variable declaration>)
                          | ('var' | 'const') <identifier> ':' <type> (';' | ',' <variable declaration>)
                          | ('var' | 'const') <identifier> ':' <type> '=' <expression> (';' | ',' <variable declaration>)

<return statement>      ::= 'return' <expression> ';'
<no-op statement>       ::= ';' $ignore

# ---------------- [ Expressions ] ----------------

<expression>            ::= <literal expression>
                          | <indexing expression>
                          | <function-call expression>
                          | <binary expression>
                          | <unary expression>
                          | <ternary expression>

<literal expression>    ::= <identifier>
                          | <string>
                          | <number>

<indexing expression>   ::= <identifier> '[' <expression> ']'

<function-call expression>  ::= <identifier> '(' [<expression> [',' <expression>]* ]* ')'

<binary expression>     ::= <expression> <binary operator> <expression>

<unary expression>      ::= <unary operator> <expression>

<assignment expression> ::= <identifier> <assignment operator> <expression>

<conditional expression>::= <expression> <conditional operator> <expression>

<ternary expression>    ::= <conditional expression> '?' <expression> ':' <expression>

# ---------------- [ Types ] ----------------

<type>                  ::= <primitive type>

<primitive type>        ::= 'byte' | 'short' | 'int' | 'long'
                          | 'ubyte' | 'ushort' | 'uint' | 'ulong'
                          | 'float' | 'double' | 'decimal'
                          | 'char' | 'string'
                          | 'bool' | 'void'

# ---------------- [ Operator ] ----------------

<binary operator>       ::= '+' | '-' | '*' | '/' | '%' | '**' | '<<' | '>>'
<unary operator>        ::= '+' | '-' | '!' | '++' | '--'
<assignment operator>   ::= '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '<<=' | '>>=' | '&=' | '^=' | '|='
<conditional operator>  ::= '==' | '!=' | '<' | '>' | '<=' | '>=' | '&' | '^' | '|' | '&&' | '||'

# ---------------- [ Other Stuff ] ----------------

<identifier>            ::= [_a-zA-Z]* [a-zA-Z]+
<string>                ::= '"' ('\.')* '"'
<number>                ::= [0-9]+

<whitespace>            ::= ' ' | '\t' | '\r' | '\n' $ignore
```