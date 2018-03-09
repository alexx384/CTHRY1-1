#include "VarStor.h"

void VarStor::addVar(std::string & VarName, Polynomial * EntireVar)
{
	this->Stor.emplace(VarName, EntireVar);
}

Polynomial * VarStor::getVarByName(std::string & VarName)
{
	std::map<std::string, Polynomial *>::iterator it;

	it = Stor.find(VarName);

	//Find variable
	if (it != Stor.end())
	{
		return it->second;
	}
	//Didn't find variable
	else
	{
		return nullptr;
	}
}
