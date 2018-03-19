#include "ErrorProcess.h"
#include "assert.h"
#include <Windows.h>

////////////
// find error position
unsigned int CheckForErrorAtTmpFile(FILE *fin);

/* Main function that parse file with comments */
void showErrorPos(const char *nameOfFileWithComment, FILE *fileWithoutComment)
{
	unsigned int ResArr[2] = { 0 };
	unsigned int CmdNum = CheckForErrorAtTmpFile(fileWithoutComment);

	FILE * fileWithComment;

	if (0 != fopen_s(&fileWithComment, nameOfFileWithComment, "rb"))
	{
		printf("Could not open file '%s'. Exitting\n", nameOfFileWithComment);
	}

	unsigned int LineNum = 0;
	unsigned int LinePos = 0;

	unsigned int i = 0;
	unsigned int CmdCnt = 0;
	int chr = 0;
	for (; (chr != EOF) && (CmdCnt < CmdNum) ; i++, LinePos++)
	{
		chr = fgetc(fileWithComment);
		switch (chr)
		{
		case '\n':
		{
			LineNum++;
			LinePos = 0;
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
				if (skipLineCom(fileWithComment))
					break;
			}
			else if (chr == '*')
			{
				if (skipCom(fileWithComment))
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

	for (; (chr = getc(fileWithComment)) == '\n'; LineNum++)
		;

	fseek(fileWithComment, -1, SEEK_CUR);

	printf("Number of Error line: %d\n", LineNum + 1);
	fclose(fileWithComment);
}

//Return count of commands which end by ';'
unsigned int CheckForErrorAtTmpFile(FILE *fin)
{
	unsigned int FilePos = ftell(fin);
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


/* If error with brackets, then it shows the error */
//void CheckForBrackets(long PosStartLine, FILE * fin)
//{
//	unsigned int BracketStack = 0;
//
//	/* Check that the inFile is EOF
//	It is necessary because the file stream is closed or paused
//	and we couldn't read or seek anymore*/
//	if (feof(fin))
//	{
//		/* Clear EOF flags and set start position */
//		rewind(fin);
//	}
//
//	fseek(fin, PosStartLine, SEEK_SET);
//
//	int chr = 0;
//
//	for (long i = PosStartLine; (chr != ';') && (chr != EOF); i++)
//	{
//		chr = getc(fin);
//
//		if (chr == '(')
//		{
//			BracketStack++;
//		}
//
//		if (chr == ')')
//		{
//			/* If ')' is redundant */
//			if (BracketStack == 0)
//			{
//				printf("Error: redundant bracket ')' \n");
//				return;
//			}
//
//			BracketStack--;
//		}
//	}
//
//	/* If not all brackets were closed */
//	if (BracketStack != 0)
//	{
//		printf("Error: redundant bracket '(' \n");
//		return;
//	}
//}

/* Show error expression */
/*void ShowErrorLine(FILE * fin)
{
	int chr = 0;

	unsigned int StartPos = ftell(fin);

	int CorrectionSlashR = 0;

	unsigned int i = 0;
	for (; (chr != EOF) && (chr != ';'); i++)
	{
		chr = fgetc(fin);

		if (chr == '/')
		{
			chr = fgetc(fin);
			i++;

			if (chr == '/')
			{
				if (skipLineCom(fin))
					break;
				i = ftell(fin) - StartPos;
			}
			else if (chr == '*')
			{
				if (skipCom(fin))
					break;
				i = ftell(fin) - StartPos;
			}
		}

		if (chr == '\n')
		{
			CorrectionSlashR ++;
		}

		if (chr == EOF)
		{
			rewind(fin);

			fseek(fin, StartPos + i, SEEK_SET);
		}
	}

	unsigned int EndPos = ftell(fin);

	fseek(fin, StartPos, SEEK_SET);
	
	char *tmp_buf = new char[EndPos - StartPos + 1];

	printf("The error instruction:\n");

	fread(tmp_buf, 1, EndPos - StartPos, fin);
	fwrite(tmp_buf, 1, EndPos - StartPos, stdout);
	putchar('\n');
}*/

void colorTextOut(FILE* f, int cnt)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	assert(hStdout != INVALID_HANDLE_VALUE, "Fatal: error while getting input handle");

	//sets the color to intense red on blue background
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

	for (int i = 0; i < cnt; i++)
		putchar(fgetc(f));
	
	//reverting back to the normal color
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
bool skipCom(FILE* f)
{
	int c = fgetc(f);

	while (1)
	{
		while (c != '*' && c != EOF)
			c = fgetc(f);

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
			e += "Fatal: unable to read from " + std::string(path_in) + "\n";
		else
			fclose(in);

		if (b2 == 0)
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
			fputc(c, out);

		c = fgetc(in);
	}

	fclose(in);
	fclose(out);
	assert(status, "Syntax error: unexpected end of file, buf no */ found");
}

/////////////----------- comments