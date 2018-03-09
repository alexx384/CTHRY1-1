#include <stdio.h>
#include <ctype.h>
#include <string>

#include "YACC\parse.cpp.h"

#define YYPARSE_PARAM
extern int yyparse(YYPARSE_PARAM);
extern Polynomial polyResult;

FILE* fin = nullptr;
void showError();


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
		int c;

		// the main block of processing
		while (yyparse() == 0)
		{
			c = fgetc(fin);
			while (c == ' ' || c == '\n')
				c = fgetc(fin);

			ungetc(c, fin);

			if (c == EOF)
				break;

			//// this is the way how to create variable
			//polyResult.assignName("Lambda");
			//
			//printf("Name = %s\nResult = %s\n", polyResult.getName(), polyResult.out().c_str());
		}

	}

	catch (std::exception e) 
	{ 
		if (e.what() != nullptr)
			printf("Unhandled exception occured: %s\n", e.what());

		printf("\nStopping execution of '%s' due to errors\n", argv[1]); 
	}

	fclose(fin);
	
#ifdef _DEBUG 
	printf("Press any key to exit\n");
	getchar();
#endif // _DEBUG 

	return 0;
}

int yylex()
{
	int c = fgetc(fin);
	while (c == ' ' || c == '\n')
		c = fgetc(fin);
	
	if (c == ';')
		return 0;

	if (isalpha(c))
	{
		yylval.int_t = c;
		return CHAR;
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

		if (*(cp - 1) == '.')
			return '.';

		*cp = ' ';
		if (cp - buf >= BSZ)
		{
			printf("Error: too large constant\n");
			return '.';
		}
		else
			ungetc(c, fin);

		yylval.real_t = atof(buf);
		return yylval.real_t == (double)(int)yylval.real_t ? INT : REAL;
	}

	
	return c;
}

void yyerror(const char *s)
{
	printf("Error while parsing: '%s'\n", s);
	showError();
}



////////////
// find error position

void showError()
{
	unsigned int FilePos = ftell(fin);
	rewind(fin);

	int chr;

	unsigned int LineNum = 0;
	unsigned int LinePos = 0;

	for (unsigned int i = 0; i < FilePos; i++, LinePos++)
	{
		chr = fgetc(fin);
		if (chr == '\n')
		{
			chr = fgetc(fin);

			//If Windows line ending like '\n\r'
			if (chr == '\r')
			{
				chr = fgetc(fin);
			}

			++LineNum;
			LinePos = 0;
		}

		if (chr == EOF)
		{
			printf("Error while obtaining error\n");
			return;
		}
	}

	printf("The error character is: '%c'\nOn line '%u' with position '%u'\n", (char)chr, LineNum + 1, LinePos);
}

