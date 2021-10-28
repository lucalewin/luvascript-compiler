# Luvascript Syntax in Backus-Naur Form


#### Version 0.1.0
```bnf

<external-function> ::= extern function <identifier> ( <parameter-list> ) : <type-identifier> <compound-statement>;

<statement> ::= <expression-statement>
			  | <jump-statement>;

<expression-statement> ::= <assignment-expression>;

<jump-statement> ::= $TODO

<assignment-expression> ::= <ternary-expression>
						  | <unary-expression> <assignment-operator> <assignment-expression>
						  ;

<ternary-expression> ::= <logical-or-expression>
					   | <logical-or-expression> ? <expression> : <ternary-expression>
					   ;

<unary-expression> ::= <post-fix-expression>
					 | ++ <unary-expression>
					 | -- <unary-expression>
					 ;

<postfix-expression> ::= <primary-expression>
					   | <postfix-expression> ( <assignment-expression> )
					   | <postfix-expression> ++
					   | <postfix-expression> --
					   ;

<primary-expression> ::= <identifier>
					   | <number>
					   | <string>
					   ;

```