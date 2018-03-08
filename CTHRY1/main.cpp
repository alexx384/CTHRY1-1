#include <stdio.h>
#include <ctype.h>
#include <string>

#include "YACC\parse.cpp.h"

#define YYPARSE_PARAM
extern int yyparse(YYPARSE_PARAM);
extern Polynomial polyResult;

FILE* fin = nullptr;

void usage()
{
	printf("Usage: program.exe <file>\n");
}

int main(int argc, char** argv)
{
	printf("Polynomial Language Interpreter is running\n\n");

	if (argc != 2)
	{
		usage();
		return 1;
	}

	if (0 != fopen_s(&fin, argv[1], "r"))
	{
		printf("Could not open file '%s'. Exitting\n", argv[1]);
		return 2;
	}

	try
	{
		// the main block of processing
		if (yyparse() == 0)
		{
			// this is the way how to create variable
			polyResult.assignName("Lambda");

			printf("Name = %s\nResult = %s\n", polyResult.getName(), polyResult.out().c_str());
		}

	}

	catch (std::exception) { printf("Stopping execution of '%s' due to errors\n", argv[1]); }

	fclose(fin);
	
#ifdef _DEBUG 
	printf("Press any key to exit\n");
	getchar();
#endif // _DEBUG 

	return 0;
}

int yylex()
{
	int c;
	while ((c = fgetc(fin)) == ' ');
	
	if (c == EOF)
		return 0;

	if (isalpha(c))
	{
		yylval.int_t = c;
		return ALPHA;
	}
	
	// get float numbers
	if (isdigit(c)) 
	{
		const int BSZ = 50;

		char buf[BSZ + 1], *cp = buf;
		int dot = 0;
	
		for (; (cp - buf) < BSZ; ++cp, c = fgetc(fin)) 
		{
			*cp = c;
			if (isdigit(c))
				continue;
			if (c == '.') 
			{
				if (dot++)
					return ('.'); // syntax error
				continue;
			}

			// end of number
			break;
		}

		*cp = ' ';
		if (cp - buf >= BSZ)
		{
			printf("Error: too large constant\n");
			return '.';
		}
		else
			ungetc(c, fin);

		yylval.real_t = atof(buf);
		return (CONST);
	}

	
	return c;
}

void yyerror(const char *s)
{
	printf("Error while parsing: '%s'\n", s);
}