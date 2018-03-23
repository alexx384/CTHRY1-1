#pragma once

#include <map>
#include <memory.h>
#include "Poly.h"

// variable storage
class VarStor
{
public:

	VarStor() = default;

	void addVar(const std::string& VarName, const Polynomial& EntireVar);	

	Polynomial* getVarByName(const std::string& VarName);

	bool variableExists(std::string key)
	{
		return this->Stor.find(key) != Stor.end();
	}

private:

	std::map<const std::string, Polynomial> Stor;
};

// basic function for calculation
Polynomial calculate(Polynomial a, Polynomial b, char op);

// create variable and mark it as not initialized
Polynomial createVariable(const std::string name);

// assign value of right poly variable to left poly variable
Polynomial assignVar(Polynomial& leftPoly, Polynomial rightPoly);

void set_stream(const std::string stream);

void output(const char* s);

void output(Polynomial& x);

void output(int x, int ch);
