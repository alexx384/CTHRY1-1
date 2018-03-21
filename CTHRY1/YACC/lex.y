%token REAL INT ALPHACHAR VAR CSTR
%start operation

////////////////------------------- inline C code

%{
	#include "../VarStor.h"
	#define MAXSZ 255
	
	// buf for input string
	char gBuf[MAXSZ + 1] = { 0 };
	int gIndex = 0;
	
	// append one char
	void AppendBuffer(int app);
	
	// clear buf
	void ClearBuffer();
	
	// return buf data as string 
	const char* GetBuffer();	
%}

%union 
{
	Polynomial poly_t;
	
	int int_t;
	double real_t;
}

%{

//#define _DEBUG_OUT

// output macros
#ifdef _DEBUG_OUT

#define debug_out_p(val, s) \
printf("%s: %s [%s]\n", s, val##.getName().c_str(), val##.out().c_str())

#else
#define debug_out_p(val, s) 

#endif

#define unused(x) x = 0

#include <malloc.h>

extern void yyerror(const char *s);
extern int yylex();

%}


////////////////------------------- inline C code

//-- simple operands 

%type <real_t> REAL
%type <real_t> INT
%type <int_t> ALPHACHAR

%type <poly_t> primary

//-- simple operands 


//-- one operation between simple operands
%type <int_t> operation

//-- possible operations between simple operands
%type <poly_t> expr_equal       
%type <poly_t> expr_add
%type <poly_t> expr_mul
%type <poly_t> expr_pow

// io operator
%type <int_t> operator
%type <int_t> out_operator

%%

/////////////------------ io operators

operator: out_operator { unused($$); }

// out operator
out_operator: '$'VAR 								{ unused($$); }

// out expression
out_operator: out_operator '<''<' expr_equal 		{ unused($$); output($4); }

// out const string
out_operator: out_operator '<''<' CSTR 				{ unused($$); output(GetBuffer()); }

// out \n
out_operator: out_operator '<''<' INT '$'		 	{ unused($$); output(int($4), '\n'); 			}
out_operator: out_operator '<''<' '$' 				{ unused($$); output(int(1),  '\n'); 			}


/////////////------------ io operators


///////////////------------///////////---- main block

// empty operator
operation: ';' { unused($$); }

// io operator
operation: operator ';' { unused($$); }

// expression
operation: expr_equal ';' { unused($$); }

// combination
operation: operation operator ';' { unused($$); }
operation: operation expr_equal ';' { unused($$); }
operation: operation ';' { unused($$); }


///////////////------------///////////---- main block




////////////////////////////------ expr
// binary and unary operations

//------- priority =

expr_equal:	expr_add '=' expr_equal	{ $$ = assignVar($1, $3); debug_out_p($$, "bin '='");}
expr_equal: expr_add;

//------- priority =


//------- priority + -

expr_add: 	expr_mul;
expr_add:	expr_add '+' expr_mul 		{$$ = calculate($1, $3, '+'); debug_out_p($$, "bin '+'");}
expr_add:	expr_add '-' expr_mul 		{$$ = calculate($1, $3, '-'); debug_out_p($$, "bin '-'");}

//------- priority + -



//------- priority * /

// it's possible to multiply number on letter: 2a, ab
expr_mul:	expr_mul ALPHACHAR 				{$$ = calculate($1, Polynomial(1, $2), '*'); debug_out_p($$, "CHAR bin '*'");}

expr_mul:	expr_pow;
expr_mul:	expr_mul '*' expr_pow 		{$$ = calculate($1, $3, '*'); debug_out_p($$, "bin '*'");}

//------- priority * /



//------- priority ^
expr_pow: 	primary;
expr_pow:	primary '^' expr_pow 		{$$ = calculate($1, $3, '^'); debug_out_p($$, "bin '^'");}

expr_pow: 	'-' primary 				{$$ = calculate(Polynomial(0), $2, '-'); debug_out_p($$, "unary '-'"); }
expr_pow:	'-' primary '^' expr_pow 	{$$ = calculate(Polynomial(0), calculate($2, $4, '^'), '-'); debug_out_p($$, "bin '^'; unary '-'"); }

//------- priority ^


//------- highest priority: number, letter

// it's a letter or real/int number
primary:	ALPHACHAR					{ $$ = Polynomial(1, $1); debug_out_p($$, "CHAR"); }
primary: 	REAL 						{ $$ = Polynomial($1);    debug_out_p($$, "REAL"); }
primary: 	INT 						{ $$ = Polynomial($1);    debug_out_p($$, "INT"); }

// it's a variable
primary:	VAR							{ $$ = createVariable(GetBuffer()); debug_out_p($$, "Variable"); }

// it's an expression in brackets
primary: 	'(' expr_add ')' 			{ $$ = $2; debug_out_p($$, "(Term)"); }

////////////////////////////------ expr	

%%

void AppendBuffer(int app)
{
	assert(gIndex < MAXSZ, "Input string is too long");
	gBuf[gIndex++] = app;
}

void ClearBuffer()
{
	gIndex = 0;
	memset(gBuf, 0, MAXSZ);
}

const char* GetBuffer()
{
	return gBuf;
}