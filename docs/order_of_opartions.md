## Order of operations

| Precedence | Operator | Description | Associativity |
|:-:|:-|:-|:-|
| 1 | ++  -- | Suffix/postfix increment and decrement | left-to-right |
| 2 | ++  -- | Prefix increment and decrement | right-to-left |
| 3 | ** | power | right-to-left |
| 4 | *  /  % | Multiplication, division, and remainder | left-to-right |
| 5 | +  - | Addition and subtraction | left-to-right |
| 6 | <<  >> | Bitwise left shift and right shift | left-to-right |
| 7 | <  <= <br> >  >= | For relational operators < and ≤ respectively <br> For relational operators > and ≥ respectively | left-to-right |
| 8 | == != | For relational = and ≠ respectively | left-to-right |
| 9 | & | Bitwise AND | left-to-right |
| 10 | ^ | Bitwise XOR (exclusive or) | left-to-right |
| 11 | \| | Bitwise OR (inclusive or) | left-to-right |
| 12 | && | Logical AND | left-to-right |
| 13 | \|\| | Logical OR | left-to-right |
| 14 | ?: | Ternary conditional | right-to-left |
| 15 | = <br> += -= <br> *= /= %= <br> <<= >>= <br> &= ^= \ | Simple assignment <br> Assignment by sum and difference <br> Assignment by product, quotient, and remainder <br> Assignment by bitwise left shift and right shift <br> Assignment by bitwise AND, XOR, and OR | right-to-left |
| 16 | , | Comma | left-to-right |