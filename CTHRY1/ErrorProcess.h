#pragma once

#ifndef ERRORPROCESS_H
#define ERRORPROCESS_H

#define CODEFILE "ParserTmpFile.txt"

#include <cstdio>
#include <string>
#include <iostream>

void showErrorPos(const char *FileWithComment, FILE *fin);
bool skipLineCom(FILE* f);
bool skipCom(FILE* f);

#endif // !ERRORPROCESS_H
