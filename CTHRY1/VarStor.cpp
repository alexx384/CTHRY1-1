#include "VarStor.h"
#include "Assert.h"

void VarStor::addVar(const std::string& VarName, const Polynomial& EntireVar)
{
	VarPoly var = { EntireVar , false };
	this->Stor.emplace(VarName, var);
}

VarPoly* VarStor::getVarByName(const std::string& VarName)
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
	VarPoly* var = gVarStorage.getVarByName(name);
	
	if (var)
	{
		var->is_initialized = true;
		var->poly = val;
	}
}

Polynomial createVariable(std::string& name)
{
	Polynomial x(0);

	if (gVarStorage.variableExists(name))
		x = gVarStorage.getVarByName(name)->poly;
	
	else
	{
		gVarStorage.addVar(name, x);
		x.assignName(name);
	}
	
	name.clear();
	return x;
}

Polynomial assignVar(Polynomial& leftPoly, Polynomial rightPoly)
{
	const std::string& leftName = leftPoly.getName();
	const std::string& rightName = rightPoly.getName();

	try
	{
		// value can not be assigned to variable without a name
		assert(!leftName.empty(), "Value can not be assigned to left operand");

		const VarPoly* right_var = gVarStorage.getVarByName(rightName);

		if (right_var)
		{
			// if right operand is variable it must be initialized
			if (!rightName.empty())
				assert(right_var->is_initialized, "Right operand was used without being initialized");
		}

		leftPoly.getValue() = rightPoly.getValue();
		updateStorage(leftName, leftPoly);
	}

	catch (std::exception e)
	{
		std::string poly_left = leftName.empty() ? leftPoly.out() : "_" + leftName + "_";
		std::string poly_right = rightName.empty() ? rightPoly.out() : "_" + rightName + "_";

		printf("Assignment exception occured: %s\n", e.what());
		printf("Operation [ %s ] = [ %s ]\n", poly_left.c_str(), poly_right.c_str());
		throw std::exception();
	}

	return leftPoly;
}

//////----------

void check(Polynomial& leftPoly, Polynomial& rightPoly)
{
	const std::string& leftName = leftPoly.getName();
	const std::string& rightName = rightPoly.getName();
	const VarPoly* right_var = gVarStorage.getVarByName(rightName);
	const VarPoly* left_var = gVarStorage.getVarByName(leftName);

	// this is a variable
	if (!leftName.empty())
		assert(left_var->is_initialized, "Left operand was used without being initialized");

	if (!rightName.empty())
		assert(right_var->is_initialized, "Right operand was used without being initialized");
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
		std::string poly_a = a.out();
		std::string poly_b = b.out();

		printf("Calculation exception occured: %s\n", e.what());
		printf("Operation: [ %s ] %c [ %s ]\n",
			poly_a.c_str(), op, poly_b.c_str());

		throw std::exception();
	}

	updateStorage(c.getName(), c);
	return c;
}

#pragma endregion Calculation