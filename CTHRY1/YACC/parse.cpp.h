#include "../VarStor.h"
typedef struct 
{
	Polynomial poly_t;
	
	int int_t;
	double real_t;
} YYSTYPE;
#define	REAL	258
#define	INT	259
#define	CHAR	260
#define	STRING	261


extern YYSTYPE yylval;
