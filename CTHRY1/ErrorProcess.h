#pragma once

#define CODEFILE "ParserTmpFile.txt"
#include <stdio.h>

void showErrorPos(const char *FileWithComment, FILE *fin);
void showErrorLine(FILE * fin);

void removeComments(const char*, const char*);
bool skipLineCom(FILE* f);
bool skipCom(FILE* f, unsigned* cnt = nullptr);