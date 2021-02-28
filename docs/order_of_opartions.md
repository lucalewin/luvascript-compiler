# Order of operations

<table>
<thead>
<tr>
<th>Precedence</th>
<th>Operator</th>
<th>Description</th>
<th>Associativity</th>
</tr>
</thead>
<tbody>
<tr>
<td>1</td>
<td>++ --</td>
<td>Suffix/postfix increment and decrement</td>
<td>left-to-right</td>
</tr>

<tr>
<td>2</td>
<td>++ --</td>
<td>Prefix increment and decrement</td>
<td rowspan=2>right-to-left</td>
</tr>

<tr>
<td>3</td>
<td>**</td>
<td>Power</td>
</tr>

<tr>
<td>4</td>
<td>* / %</td>
<td>Multiplication, division, and remainder</td>
<td>left-to-right</td>
</tr>

<tr>
<td>5</td>
<td>+ -</td>
<td>Addition and subtraction</td>
<td rowspan=10>left-to-right</td>
</tr>

<tr>
<td>6</td>
<td><< >></td>
<td>Bitwise left shift and right shift</td>
</tr>

<tr>
<td>7</td>
<td> < <= <br> > >= </td>
<td>For relational operators < and ≤ respectively <br>
For relational operators > and ≥ respectively</td>
</tr>

<tr>
<td>8</td>
<td>== !=</td>
<td>For relational = and ≠ respectively</td>
</tr>

<tr>
<td>9</td>
<td>&</td>
<td>Bitwise AND</td>
</tr>

<tr>
<td>10</td>
<td>^</td>
<td>Bitwise XOR (exclusive or)</td>
</tr>

<tr>
<td>11</td>
<td>|</td>
<td>Bitwise OR (inclusive or)</td>
</tr>

<tr>
<td>12</td>
<td>&&</td>
<td>Logical AND</td>
</tr>

<tr>
<td>13</td>
<td>||</td>
<td>Logical OR</td>
</tr>

<tr>
<td>14</td>
<td>? :</td>
<td>Ternary conditional</td>
</tr>

<tr>
<td>15</td>
<td> = <br> += -= <br> *= /= %= <br> <<= >>= <br> &= ^= \</td>
<td>Simple assignment <br>
Assignment by sum and difference <br>
Assignment by product, quotient, and remainder <br>
Assignment by bitwise left shift and right shift <br>
Assignment by bitwise AND, XOR, and OR</td>
<td>right-to-left</td>
</tr>

<tr>
<td>16</td>
<td>,</td>
<td>Comma</td>
<td>left-to-right</td>
</tr>
</tbody>
</table>