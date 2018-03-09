#include "../Poly.h"
typedef struct 
{
	// Var
	Polynomial poly_t;
	std::string string_t;

	// char string real int
	int int_t;
	double real_t;
} YYSTYPE;
#define	REAL	258
#define	INT	259
#define	CHAR	260


extern YYSTYPE yylval;
