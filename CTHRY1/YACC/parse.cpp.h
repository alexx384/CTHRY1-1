#include "../Poly.h"
typedef struct 
{
	Polynomial poly_t;
	std::string string_t;
	int int_t;
	double real_t;
} YYSTYPE;
#define	REAL	258
#define	INT	259
#define	CHAR	260
#define	IEND	261


extern YYSTYPE yylval;
