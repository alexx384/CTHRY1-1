#include "VarStor.h"
typedef struct 
{
	Polynomial poly_t;
	
	int int_t;
	double real_t;
} YYSTYPE;
#define	REAL	258
#define	INT	259
#define	ALPHACHAR	260
#define	VAR	261
#define	CSTR	262


extern YYSTYPE yylval;
