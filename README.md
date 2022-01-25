

## Compilation

```gcc befunge.c -o befunge``` or ```make```


## What is this?

Befunge is an esoteric stack-based self modifying programming language. Instead of a program counter in one dimention, instructions in Befunge are placed in 2D. Here is an example factorial code (from [esolangs](https://esolangs.org/wiki/Befunge#Factorial))
```
&>:1-:v v *_$.@
 ^    _$>\:^  
```
The program counter starts at the top left corner and moves towards the right by default. The first instruction is '&' (number input). The direction of the program counter is altered with ```>, v, <, ^``` to make a loop that essentially subtracts by 1 and multiplies until the factorial is calculated. Details of each instruction is given in the instruction list below.

## Instruction list (from [Wikipedia](https://en.wikipedia.org/wiki/Befunge))
| Instruction | Description |
|:-----------:|:------------|
| 0-9 |	Push this number on the stack |
| + |	Addition: Pop a and b, then push a+b |
| - |	Subtraction: Pop a and b, then push b-a |
| * |	Multiplication: Pop a and b, then push a*b |
|/  |	Integer division: Pop a and b, then push b/a, rounded towards 0. |
|%  |	Modulo: Pop a and b, then push the remainder of the integer division of b/a. |
|!  |	Logical NOT: Pop a value. If the value is zero, push 1; otherwise, push zero. |
|`  |	Greater than: Pop a and b, then push 1 if b>a, otherwise zero. |
|>  |	Start moving right |
|<  |	Start moving left |
|^  |	Start moving up |
|v  |	Start moving down |
|?  |	Start moving in a random cardinal direction |
|_  |	Pop a value; move right if value=0, left otherwise |
|\| |	Pop a value; move down if value=0, up otherwise |
|"  |	Start string mode: push each character's ASCII value all the way up to the next " |
|:  |	Duplicate value on top of the stack |
|\  |	Swap two values on top of the stack |
|$  |	Pop value from the stack and discard it |
|.  |	Pop value and output as an integer followed by a space |
|,  |	Pop value and output as ASCII character |
|#  |	Bridge: Skip next cell |
|p  |	A "put" call (a way to store a value for later use). Pop y, x, and v, then change the character at (x,y) in the program to the character with ASCII value v |
|g  |	A "get" call (a way to retrieve data in storage). Pop y and x, then push ASCII value of the character at that position in the program |
|&  |	Ask user for a number and push it |
|~  |	Ask user for a character and push its ASCII value |
|@  |	End program |
|(space) |	No-op. Does nothing |
