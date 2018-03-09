#include "Common.h"

void showError(FILE *inFile, unsigned int FilePos)
{
	rewind(inFile);

	int chr;

	unsigned int LineNum = 0;
	unsigned int LinePos = 0;

	for (unsigned int i = 0; i < FilePos; i++, LinePos++)
	{
		chr = fgetc(inFile);
		if (chr == '\n')
		{
			chr = fgetc(inFile);

			//If Windows line ending like '\n\r'
			if (chr == '\r')
			{
				chr = fgetc(inFile);
			}

			++LineNum;
			LinePos = 0;
		}

		if (chr == EOF)
		{
			std::cout << "Error while obtaining error" << std::endl;
			return;
		}
	}

	std::cout << "The error character is: " << (char)chr << std::endl
		<< "On line " << LineNum
		<< " with position " << LinePos << std::endl;
}