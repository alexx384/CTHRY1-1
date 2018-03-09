#pragma once

#ifndef VARSTOR_H
#define VARSTOR_H

#include <map>
#include <string>
#include "Poly.h"

class VarStor
{
public:
	VarStor() = default;

	void addVar(std::string & VarName, Polynomial * EntireVar);	

	Polynomial * getVarByName(std::string & VarName);

private:
	std::map<std::string, Polynomial *> Stor;
};

#endif // !VARSTOR_H