#include <cstdio>
#include <ctype.h>
#include <string>

#include "ErrorProcess.h"

#include "YACC\parse.cpp.h"

#define CODEFILE "ParserTmpFile.txt"

#define YYPARSE_PARAM
extern int yyparse(YYPARSE_PARAM);

unsigned gInstruction = 0;

FILE* fin = nullptr;
void removeComments(const char*, const char*);

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
		std::cout << (char)yychar << std::endl;

		if (strlen(e.what()) > 0)
			printf("Exception occured: %s\n", e.what());

		showErrorPos(argv[1], fin);
	}


	//If it needs
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
extern std::string GetBuffer();

//////////////////////////

bool isallowed(char c)
{
	char allowed[] = { '#', '@', '!', '?', '~', '%', '&', '.', ',' , ':'};

	for (int i = 0; i < sizeof(allowed); i++)
		if (allowed[i] == c)
			return true;

	return false;
}

int yylex()
{
	int c = fgetc(fin);

	//if (c == ';')
	//	return 0;

	/* get value of types string char real int
	if the value in buffer is real or int then it will be converted to yylval < double | int >
	idea - values of int, real can be stored in buffer and in yylval < double | int > simultaneously
	then we won't need any convertions int <-> string in he future 
	*/

	if (isalnum(c) || isallowed(c))
	{
		ClearBuffer();

		while (isalnum(c) || isallowed(c))
		{
			AppendBuffer(c);
			c = fgetc(fin);
		}

		// return last char
		ungetc(c, fin);

		std::string str = GetBuffer();

		try
		{
			// is it a real or int
			if (strchr(str.c_str(), '.'))
			{
				// real in buffer - e.g. 345.345
				yylval.real_t = std::stod(str);
				return REAL;
			}

			else
			{
				// int in buffer - e.g. 345
				yylval.real_t = std::stoi(str);
				return INT;
			}
		}

		catch (...)
		{
			// string in buffer
			// this is one char: a-z A-Z; else - a string such as 12.23.2222asasc#?
			if (str.length() == 1)
			{
				yylval.int_t = str[0];
				return CHAR;
			}
			else
				return STRING;
						
		}
	}
	
	return c;
}

void yyerror(const char *s)
{
	throw std::exception("Syntax error:");

}

/////////////----------- comments /**/, //

/* begin from //
if we are reached EOF then return TRUE otherwise return FALSE */
bool skipLineCom(FILE* f)
{
	int c = fgetc(f);
	while (c != '\n' && c != EOF)
	{
	//	fputc(' ', o);
		c = fgetc(f);
	}

	if (c == '\n')
	{
	//	fputc(c, o);
		return false;
	}

	return true;
}

/* begin from /*
if we are reached EOF then return TRUE otherwise return FALSE */
bool skipCom(FILE* f)
{
	int c = fgetc(f);

	while (1)
	{
		while (c != '*' && c != EOF)
		{
	//		fputc(' ', o);
			c = fgetc(f);
		}

		if (c == EOF)
			return true;

		c = fgetc(f);
		if (c == '/')
			break;
	}

	return false;
}

// path_in with comments -> path_out without comments
void removeComments(const char* path_in, const char* path_out)
{
	FILE *in, *out;
	errno_t b1, b2 = 0;

	b1 = !fopen_s(&in, path_in, "r");
	b2 = !fopen_s(&out, path_out, "w");

	if (b1 == 0 || b2 == 0)
	{
		std::string e = "";

		if (b1 == 0)
			e += "Unable to read from " + std::string(path_in) + "\n";
		else
			fclose(in);

		if (b2 == 0)
			e += "Unable or write to " + std::string(path_out) + "\n";
		else
			fclose(out);

		assert(0, e.c_str());
	}


	/* main code here */
	bool status = true;
	int c = fgetc(in);

	while (c != EOF)
	{
		while (isspace(c))
			c = fgetc(fin);

		if (c == '/')
		{
			int slash = c;
			c = fgetc(in);
			if (c == '/')
			{
				if (skipLineCom(in))
					break;
			}

			else if (c == '*')
			{
				if (skipCom(in))
				{
					status = false;
					break;
				}
			}

			else if (c == EOF)
			{
				fputc(slash, out);
				break;
			}

			else
			{
				fputc(slash, out);
				fputc(c, out);
			}
		}

		else
			fputc(c, out);

		c = fgetc(in);
	}

	fclose(in);
	fclose(out);
	assert(status, "Unexpected end of file, buf no */ found");
}

/////////////----------- comments