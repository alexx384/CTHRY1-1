%token REAL INT CHAR
%start operation

////////////////------------------- inline C code

%{
	#include "../VarStor.h"
%}

%union 
{
	// Var
	Polynomial poly_t;
	std::string string_t;

	// char string real int
	int int_t;
	double real_t;
}

%{

#define _DEBUG_OUT

// output macros
#ifdef _DEBUG_OUT
#define _out(val, s) printf("%s: %s\n", s, val##.out().c_str())
#else
#define _out(val, s) 
#endif

#include <malloc.h>
#include <math.h>

	Polynomial polyResult;
	
	extern void yyerror(const char *s);
	extern int yylex();
%}


////////////////------------------- inline C code

//-- simple operands 

%type <poly_t> variable 
%type <string_t> string 

%type <real_t> REAL
%type <real_t> INT
%type <int_t> CHAR

%type <poly_t> primary

//-- simple operands 


//-- one operation between simple operands
%type <int_t> operation

//-- possible operations between simple operands
%type <poly_t> expr_equal       
%type <poly_t> expr_add
%type <poly_t> expr_mul
%type <poly_t> expr_pow

%%
/////////////------------ strings and variables
// string is a sequence of numeric letters and alpha letters

string: CHAR 		{ $$ = std::string(1, $1); 	}
string: string CHAR { $$ += std::string(1, $2); }

string: INT 		{ $$ = std::to_string($1);	}
string: string INT 	{ $$ += std::to_string($2); }

variable: '_' string '_' { $$ = createVariable($2); }

/////////////------------ strings and variables


///////////---- start here

operation: expr_equal /* explicitly ';' */ { $$ = 0; }
operation: expr_equal '<' expr_equal { $$ = 0; };
operation: expr_equal '>' expr_equal { $$ = 0; };

////////////////////////////------ expr
// binary and unary operations

//------- priority =

expr_equal:	expr_add '=' expr_equal	{$$ = assignVar($1, $3); _out($$, "bin '='");}
expr_equal: expr_add;

//------- priority =


//------- priority + -

expr_add: 	expr_mul;
expr_add:	expr_add '+' expr_mul 		{$$ = calculate($1, $3, '+'); _out($$, "bin '+'");}
expr_add:	expr_add '-' expr_mul 		{$$ = calculate($1, $3, '-'); _out($$, "bin '-'");}

//------- priority + -



//------- priority * /

// it's possible to multiply number on letter: 2a, ab
expr_mul:	expr_mul CHAR 				{$$ = calculate($1, Polynomial(1, $2), '*'); _out($$, "CHAR bin '*'");}

expr_mul:	expr_pow;
expr_mul:	expr_mul '*' expr_pow 		{$$ = calculate($1, $3, '*'); _out($$, "bin '*'");}

//------- priority * /



//------- priority ^
expr_pow: 	primary;
expr_pow:	primary '^' expr_pow 		{$$ = calculate($1, $3, '^'); _out($$, "bin '^'");}

expr_pow: 	'-' primary 				{$$ = calculate(Polynomial(0), $2, '-'); _out($$, "unary '-'"); }
expr_pow:	'-' primary '^' expr_pow 	{$$ = calculate(Polynomial(0), calculate($2, $4, '^'), '-'); _out($$, "bin '^'; unary '-'"); }

//------- priority ^


//------- highest priority: number, letter

// it's a letter or real/int number
primary:	CHAR						{ $$ = Polynomial(1, $1); _out($$, "CHAR"); }
primary: 	REAL 						{ $$ = Polynomial($1);    _out($$, "REAL"); }
primary: 	INT 						{ $$ = Polynomial($1);    _out($$, "INT"); }

// it's a variable
primary:	variable					{ $$ = $1; _out($$, "Variable"); }

// it's an expression in brackets
primary: 	'(' expr_add ')' 			{ $$ = $2; _out($$, "(Term)"); }

////////////////////////////------ expr	

%%