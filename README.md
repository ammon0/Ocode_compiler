# The OCode Compiler

I'm proving I can build my own programming language. I am not currently licencing this software in any way.

Usage: `occ [-d=debugfile] [-o=outfilename] infile.oc`

## OCode

I want a type checking mode between one type, and void (any type), such that I can list the allowable types. particularly for function calls, so one parameter can be used to interpret what the others should be. Like a union without having to wrap each parameter in a union before passing it.

## Intermediate Code
The intermetiate represetation consists of three parts:
*	The name array
*	The symbol table
*	The op-code queue

The name array is a dynamically sized array containing all the user and compiler defined names. I can also include all the program's string literals.

The symbol table contains each variable's declaration information including types and sizes and an index into the name array.

The op-code queue contains a series of three-address op-codes:

Binary Operators

OP	RSLT	ARG1	ARG2

Unary Operators

OP	RSLT	ARG

Flow Control

LBL
JMP
CJMP	CND	LBL

 # literal numbers
 % temp variables / register place holders.

## Other junk

What if all declared types were classes
	could there be implicit member selection if there is only one member?

the only fundamental operations are
	declarations
		constants
		variables
		classes
		functions
	assignments
	reference / dereference
	function calls
	label / jump
	if / else

for simplicity we need to assume only one data type: unsigned qword

First:
	variable declarations (one type)
	assignments
	expressions

Second:
	function calls

fourth:
	ref / deref (arithmetic depends on typesizes)?


Enumerated types should each be in their own namespace

REFERENCES
	With Ref operator
		variables must be implicitly dereferenced
		reference passing must be done explicitly
	Without ref operator
		all variables need to be dereferenced to get their value
		constants do not need to be dereferenced, but may for consistancy
		passing anything to a function always passes the reference allows us to make explicit whether they are in or out.

Three adress code types:
	parameter pass
	call function
	return function

