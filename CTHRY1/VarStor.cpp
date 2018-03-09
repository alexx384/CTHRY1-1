#include "VarStor.h"
#include "Assert.h"

void VarStor::addVar(const std::string& VarName, const Polynomial& EntireVar)
{
	this->Stor.emplace(VarName, EntireVar);
}

Polynomial* VarStor::getVarByName(const std::string& VarName)
{
	//Find variable
	auto it = Stor.find(VarName);

	//Didn't find variable
	if (it == Stor.end())
		return nullptr;

	return &it->second;
}


#pragma region Calculation

VarStor gVarStorage;

// when a new value is assigned to variable the variable storage must be updated with its value
// update and mark it initialized
void updateStorage(const std::string name, const Polynomial& val)
{
	Polynomial* poly = gVarStorage.getVarByName(name);
	
	if (poly)
		*poly = val;
}

Polynomial createVariable(std::string name)
{
	// empty non-initialized
	Polynomial x;

	if (gVarStorage.variableExists(name))
		x = *gVarStorage.getVarByName(name);
	
	else
	{
		x.assignName(name);
		gVarStorage.addVar(name, x);
	}
	
	return x;
}

/*
| 				|	name	|	empty list (None = true) |
| 				|			|							 |
| polynomial	|	no		|		no					 |
| 				|			|							 |
| variable		|	yes		|	yes if not initalized	 |
| 				|			|	no if initialized		 |
| 				|			|							 |
*/

Polynomial assignVar(Polynomial& leftPoly, Polynomial rightPoly)
{
	const std::string& leftName = leftPoly.getName();
	const std::string& rightName = rightPoly.getName();

	try
	{
		// value can only be assigned to variable, not polynomial
		assert(!leftName.empty(), "Value can not be assigned to left operand");

		// if rightpoly is not initialized variable
		assert(!rightPoly.None(), "Right operand was used without being initialized");

		leftPoly.getValue() = rightPoly.getValue();
		updateStorage(leftName, leftPoly);
	}

	catch (std::exception e)
	{
		std::string poly_left = leftName.empty() ? leftPoly.out() : "_" + leftName + "_";
		std::string poly_right = rightName.empty() ? rightPoly.out() : "_" + rightName + "_";

		printf("Assignment exception occured: %s\n", e.what());
		printf("Operation [ %s ] = [ %s ]\n", poly_left.c_str(), poly_right.c_str());
		throw std::exception("");
	}

	return leftPoly;
}

//////----------

void check(Polynomial& leftPoly, Polynomial& rightPoly)
{
	const std::string& leftName = leftPoly.getName();
	const std::string& rightName = rightPoly.getName();

	// poly + poly
	if (leftName.empty() && rightName.empty())
		return;

	const char *r = "Right operand was used without being initialized";
	const char *l = "Left operand was used without being initialized";

	// var + var
	if (!leftName.empty() && !rightName.empty())
	{
		assert(!leftPoly.None(), l);
		assert(!rightPoly.None(), r);
	}

	// var + poly or poly + var
	else
	{
		// poly + var
		if (leftName.empty())
		{
			assert(!rightPoly.None(), r);
		}
		else
		{
			assert(!leftPoly.None(), l);
		}
	}
}

Polynomial calculate(Polynomial a, Polynomial b, char op)
{
	Polynomial c;

	try
	{
		assert(op == '+' || op == '-' || op == '*' || op == '^', "Unknown operation");
		check(a, b);

		switch (op)
		{
		case '+':
			c = a + b;
			break;

		case '-':
			c = a - b;
			break;

		case '*':
			c = a * b;
			break;

		case '^':
			c = a ^ b;
			break;
		}
	}

	catch (std::exception e)
	{
		std::string poly_a = !a.getName().empty() ? "_" + a.getName() + "_" : a.out();
		std::string poly_b = !b.getName().empty() ? "_" + b.getName() + "_" : b.out();

		printf("Calculation exception occured: %s\n", e.what());
		printf("Operation: [ %s ] %c [ %s ]\n",
			poly_a.c_str(), op, poly_b.c_str());

		throw std::exception("");
	}

	updateStorage(c.getName(), c);
	return c;
}

#pragma endregion Calculation