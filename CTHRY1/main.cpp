#include <ctype.h>
#include <string>

#include "ErrorProcess.h"
#include "parse.cpp.h"

#define YYPARSE_PARAM
extern int yyparse(YYPARSE_PARAM);

FILE* fin = nullptr;

void usage()
{
	printf("Usage: program.exe <file>\n");
}

extern int yychar;

int main(int argc, char** argv)
{
	int c;
	printf("Polynomial Language Interpreter is running\n\n");

	if (argc != 2)
	{
		usage();
		goto main_end;
	}

	if (0 != fopen_s(&fin, argv[1], "rb"))
	{
		printf("Could not open file '%s'. Exitting\n", argv[1]);
		goto main_end;
	}

	c = fgetc(fin);
	fclose(fin);
	fin = nullptr;
	
	// empty file -> do nothing
	if (c == EOF)
		goto main_end;

	try
	{
		// launch the copy without comments 
		removeComments(argv[1], CODEFILE);

		if (0 != fopen_s(&fin, CODEFILE, "r"))
		{
			printf("Could not open file '%s'. Exitting\n", CODEFILE);
			goto main_end;
		}

		yyparse();
	}

	catch (std::exception e) 
	{
		putchar('\n');
		if (strlen(e.what()) > 0)
			printf("Exception occured: %s\n", e.what());
		
		if (fin)
		{
			showErrorLine(fin);
			showErrorPos(argv[1], fin);
		}
	}


	//If it needs
	if (fin)
		fclose(fin);
	remove(CODEFILE);
	

main_end:

#ifdef _DEBUG 
	printf("Press any key to exit\n");
	getchar();
#endif // _DEBUG 

	return 0;
}


extern void AppendBuffer(int app);
extern void ClearBuffer();
extern const char* GetBuffer();

//////////////////////////

char getUntil(FILE* f, bool(*stop_if)(int))
{
	int c = fgetc(f);
	while (!stop_if(c) && c != EOF)
	{
		AppendBuffer(c);
		c = fgetc(f);
	}
	
	return c;
}

int yylex()
{
	int c = fgetc(fin);

	/* get value of types string char real int
	if the value in buffer is real or int then it will be converted to yylval < double | int >
	idea - values of int, real can be stored in buffer and in yylval < double | int > simultaneously
	then we won't need any convertions int <-> string in he future 
	*/

	// obtain variable name
	if (c == '$')
	{
		ClearBuffer();
		c = getUntil(fin, [](int x) { return !isalnum(x); });
		ungetc(c, fin);

		// it's only '$'
		if (strlen(GetBuffer()) == 0)
			return '$';

		return VAR;
	}

	// obtain const string
	if (c == '\'')
	{
		ClearBuffer();
		getUntil(fin, [](int x) { return (x == '\''); });
		return CSTR;
	}

	if (isalpha(c))
	{
		yylval.int_t = c;
		return ALPHACHAR;
	}

	if (isdigit(c))
	{
		ClearBuffer();
		while (isdigit(c))
		{
			AppendBuffer(c);
			c = fgetc(fin);
		}

		// seems to be real
		if (c == '.')
		{
			AppendBuffer(c);
			c = fgetc(fin);
			while (isdigit(c))
			{
				c = fgetc(fin);
				AppendBuffer(c);
			}

			// return last read symbol
			ungetc(c, fin);
			yylval.real_t = std::stod(GetBuffer());
			ClearBuffer();
			return REAL;
		}

		// seems to be int
		else
		{
			ungetc(c, fin);
			yylval.real_t = std::stoi(GetBuffer());
			ClearBuffer();
			return INT;
		}

	}

	return c;
}

void yyerror(const char *s)
{
	throw std::exception("Syntax error: ");
}
