// project may be compiled in visual studio 2017
// or g++ main.cpp ErrorProcess.cpp Poly.cpp VarStor.cpp parse.cpp

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif


#include "ErrorProcess.h"
#include "assert.h"


////////////
// find error position
unsigned int CheckForErrorAtTmpFile(FILE *fin);

/* Main function that parse file with comments */
void showErrorPos(const char *nameOfFileWithComment, FILE *fileWithoutComment)
{
	unsigned int CmdNum = CheckForErrorAtTmpFile(fileWithoutComment);

	FILE * fileWithComment = fopen(nameOfFileWithComment, "r");

	if (!fileWithComment)
	{
		printf("Could not open file '%s'. Exitting\n", nameOfFileWithComment);
		return;
	}

	unsigned int LineNum = 0;

	unsigned int i = 0;
	unsigned int CmdCnt = 0;
	int chr = 0;
	for (; (chr != EOF) && (CmdCnt < CmdNum) ; i++)
	{
		chr = fgetc(fileWithComment);
		switch (chr)
		{
		case '\n':
		{
			LineNum++;
		} break;
		case ';':
		{
			CmdCnt++;
		} break;
		case '/':
		{
			chr = fgetc(fileWithComment);
			if (chr == '/')
			{
				LineNum++;
				if (skipLineCom(fileWithComment))
					break;
			}
			else if (chr == '*')
			{
				if (skipCom(fileWithComment, &LineNum))
					break;
			}
		} break;
		default:
			break;
		}

		if (chr == EOF)
		{
			rewind(fileWithComment);
			fseek(fileWithComment, i, SEEK_SET);
			printf("Error while obtaining error\n");
			return;
		}
	}

	bool isNotImportantSymbol;

	do
	{
		isNotImportantSymbol = false;

		chr = fgetc(fileWithComment);
		switch (chr)
		{
		case '\n':
		{
			LineNum++;
			isNotImportantSymbol = true;
		} break;
		case ';':
		{
			isNotImportantSymbol = true;
		} break;
		case '/':
		{
			isNotImportantSymbol = true;
			chr = fgetc(fileWithComment);
			if (chr == '/')
			{
				LineNum++;
				if (skipLineCom(fileWithComment))
					break;
			}
			else if (chr == '*')
			{
				if (skipCom(fileWithComment, &LineNum))
					break;
			}
		} break;
		}

		if (chr == EOF)
		{
			isNotImportantSymbol = false;
		}
	} while (isNotImportantSymbol);

	printf("Number of Error line: %d\n", LineNum + 1);
	fclose(fileWithComment);
}

//Return count of commands which end by ';'
unsigned int CheckForErrorAtTmpFile(FILE *fin)
{
	unsigned int FilePos = ftell(fin) - 1;
	rewind(fin);

	int chr = 0;

	unsigned int CmdNum = 0;
	unsigned int CmdPos = 0;

	unsigned int i;
	for (i = 0; (i < FilePos) && (chr != EOF); i++, CmdPos++)
	{
		chr = fgetc(fin);
		if (chr == ';')
		{
			CmdNum++;
			CmdPos = -1;
		}
	}

	if (chr == EOF)
	{
		rewind(fin);
	}

	return CmdNum;
}


void colorTextOut(FILE* f, int cnt)
{
#ifdef _WIN32
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	assert(hStdout != INVALID_HANDLE_VALUE, "Fatal: error while getting input handle");

	//sets the color to intense red on blue background
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

	for (int i = 0; i < cnt; i++)
		putchar(fgetc(f));
	
	//reverting back to the normal color
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

#else

	for (int i = 0; i < cnt; i++)
		printf("\033[1;31m%c\033[0m", fgetc(f));

#endif
}

void showErrorLine(FILE * fin)
{
	printf("Code around: ");

	unsigned markEnd = ftell(fin) - 1;
	unsigned markStart = 0;

	rewind(fin);
	
	// first find mark start position
	unsigned i = 0;
	int c = fgetc(fin);
	while (i < markEnd && c != EOF)
	{
		if (c == ';')
			markStart = i;

		i++;
		c = fgetc(fin);
	}

	// marked output 
	int delta = markEnd - markStart + 1;
	fseek(fin, -delta, SEEK_CUR);
	colorTextOut(fin, delta);
	
	// output while not ';'
	c = fgetc(fin);
	while (c != ';' && c != EOF)
	{
		putchar(c);
		c = fgetc(fin);
	}

	if (c == ';')
		putchar(c);

	putchar('\n');
}

/////////////----------- comments /**/, //

/* begin from //
if we are reached EOF then return TRUE otherwise return FALSE */
bool skipLineCom(FILE* f)
{
	int c = fgetc(f);
	while (c != '\n' && c != EOF)
		c = fgetc(f);

	if (c == '\n')
		return false;

	return true;
}

/* begin from /*
if we are reached EOF then return TRUE otherwise return FALSE */
bool skipCom(FILE* f, unsigned* cnt)
{
	int c = fgetc(f);

	while (1)
	{
		while (c != '*' && c != EOF)
		{
			if (cnt != nullptr && c == '\n')
				(*cnt)++;

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

	in = fopen(path_in, "r");
	out = fopen(path_out, "w");

	if (!in || !out)
	{
		std::string e = "";

		if (!in)
			e += "Fatal: unable to read from " + std::string(path_in) + "\n";
		else
			fclose(in);

		if (!out)
			e += "Fatal: unable or write to " + std::string(path_out) + "\n";
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
			c = fgetc(in);

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

		else if (c == '\'')
		{
			putc(c, out);
			c = fgetc(in);
			putc(c, out);
			while (c != '\'' && c != EOF)
			{
				c = fgetc(in);
				putc(c, out);
			}

			assert(c != EOF, "Syntax error: unexpected end of file, buf no ' found");
		}

		else
			if (c != EOF)
				fputc(c, out);

		c = fgetc(in);
	}

	fclose(in);
	fclose(out);
	assert(status, "Syntax error: unexpected end of file, buf no */ found");
}

/////////////----------- comments