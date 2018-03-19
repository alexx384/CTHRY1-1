#include "ErrorProcess.h"

////////////
// find error position
unsigned int CheckForErrorAtTmpFile(FILE *fin);
void CheckForBrackets(long PosStartLine, FILE * fin);
void ShowErrorLine(FILE * fin);

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

	std::cout << "Number of Error line: " << LineNum + 1 << std::endl;

	ShowErrorLine(fileWithComment);

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

	/* Processing brackets */
	long PosStartLine = i - CmdPos;

	/* if the error is due to brackets, then show error */
	CheckForBrackets(PosStartLine, fin);
	
	//fseek(fin, PosStartLine, SEEK_SET);

	return CmdNum;
}

/* If error with brackets, then it shows the error */
void CheckForBrackets(long PosStartLine, FILE * fin)
{
	unsigned int BracketStack = 0;

	/* Check that the inFile is EOF
	It is necessary because the file stream is closed or paused
	and we couldn't read or seek anymore*/
	if (feof(fin))
	{
		/* Clear EOF flags and set start position */
		rewind(fin);
	}

	fseek(fin, PosStartLine, SEEK_SET);

	int chr = 0;

	for (long i = PosStartLine; (chr != ';') && (chr != EOF); i++)
	{
		chr = getc(fin);

		if (chr == '(')
		{
			BracketStack++;
		}

		if (chr == ')')
		{
			/* If ')' is redundant */
			if (BracketStack == 0)
			{
				std::cout << "Error: redundant bracket ')'" << std::endl;
				return;
			}

			BracketStack--;
		}
	}

	/* If not all brackets were closed */
	if (BracketStack != 0)
	{
		std::cout << "Error: redundant bracket '('" << std::endl;
		return;
	}
}

/* Show error expression */
void ShowErrorLine(FILE * fin)
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

	std::cout << "The error line is: ";

	fread(tmp_buf, 1, EndPos - StartPos, fin);

	fwrite(tmp_buf, 1, EndPos - StartPos, stdout);
}
