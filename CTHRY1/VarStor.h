#pragma once

#include <map>
#include "Poly.h"

struct VarPoly
{
	Polynomial poly;	
	bool is_initialized;
};

// varialble storage
class VarStor
{
public:

	VarStor() = default;

	void addVar(const std::string& VarName, const Polynomial& EntireVar);	

	VarPoly* getVarByName(const std::string& VarName);

	bool variableExists(std::string key)
	{
		return this->Stor.find(key) != Stor.end();
	}

private:

	std::map<const std::string, VarPoly> Stor;
};

// basic function for calculation
Polynomial calculate(Polynomial a, Polynomial b, char op);

// create variable and mark it as not initialized
Polynomial createVariable(std::string& name);

// assign value of right poly variable to left poly variable
Polynomial assignVar(Polynomial& leftPoly, Polynomial rightPoly);
