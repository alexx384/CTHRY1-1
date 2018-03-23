#include "Poly.h"
#include <cmath>
#include <algorithm>

std::string doubleToString(double x)
{
	char buf[56] = { 0 };
	snprintf(buf, sizeof(buf), "%g", x);

	return std::string(buf);
}

#pragma region VAR

const bool operator == (const Var &left, const Var &right)
{
	return left.letter == right.letter && left.power == right.power;
}

const bool operator < (const Var &left, const Var &right)
{
	if (left.letter < right.letter)
		return true;
	else if (left.letter > right.letter)
		return false;
	else
	{
		if (left.power < right.power)
			return true;
		else
			return false;
	}
}

#pragma endregion VAR

#pragma region POLYELEM

PolyElem PolyElem::setToZero()
{
	if (!variableList.empty())
		variableList.clear();

	coefficient = 0;
	return *this;
}

std::string PolyElem::out(bool first)
{
	std::string s;

	if (first)
		s = doubleToString(coefficient);
	else
		s = doubleToString(abs(coefficient));

	for (auto& elem : variableList)
	{
		assert(elem.letter != 0, "Fatal: elem.letter was null at output");
		assert(elem.power != 0, "Fatal: elem.power was null at output");

		if (elem.power == 1)
			s += "*" + std::string(1, elem.letter);
		else if (elem.power > 1)
			s += "*" + std::string(1, elem.letter) + "^" + doubleToString(elem.power);
		else
			s += "*" + std::string(1, elem.letter) + "^(" + doubleToString(elem.power) + ')';
	}

	// remove: 1*x -> õ
	if (s.length() > 1 && s[0] == '1' && s[1] == '*')
		return s.substr(2);

	return s;
}

PolyElem PolyElem::operator= (const PolyElem &e)
{
	this->coefficient = e.coefficient;
	this->variableList = e.variableList;

	return *this;
}

PolyElem operator^ (const PolyElem& leftPoly, const PolyElem& rightPoly)
{
	PolyElem newPoly = leftPoly;
	return newPoly ^= rightPoly;
}

/*
Exponentiation of polynomial element, for example (11a b^2c^2)^(-2.08)
Algorithm:
1) raise the coefficient to a degree
2) for each variable multiply its current power by rightpoly.coef
If its degree was equal to 0, then remove an item from the list
*/
PolyElem operator^= (PolyElem& leftPoly, const PolyElem& rightPoly)
{
	assert(rightPoly.variableList.empty(), "Semantic error: operation 'x ^ n' where n is not number is not defined");
	assert(leftPoly.coefficient != 0, "Semantic error: operation '0 ^ n' has undefined behaviour");

	std::list<Var>& newPolyList = leftPoly.variableList;

	// (1)
	double power = rightPoly.coefficient;
	leftPoly.coefficient = std::pow(leftPoly.coefficient, power);

	// (2)
	for (auto& elem : newPolyList)
		elem.power *= power;

	// found a zero degree -> 1 -> remove from list letter
	newPolyList.remove_if([](const Var& x) { return x.power == 0; });
	return leftPoly;
}

PolyElem operator* (const PolyElem& leftPoly, PolyElem rightPoly)
{
	PolyElem newPoly = leftPoly;
	return newPoly *= rightPoly;
}

/*
Multiplication of elements in polynomial such as 111abc111a b c 11 a b^2c^2
Algorithm:
1) Multiply the coef. polynomials, and then element by element in the list left:
	1.1) if found the same letter in the right, increase the degree of the element
	in left on the degree of the element in right and remove that element from right

2) if the right list is not empty, attach elements from right to left
3) Return left (newPoly)
*/
PolyElem operator*= (PolyElem& leftPoly, PolyElem rightPoly)
{
	if (rightPoly.coefficient == 0 || leftPoly.coefficient == 0)
		return leftPoly.setToZero();

	// (1)
	leftPoly.coefficient *= rightPoly.coefficient;

	std::list<Var>& left = leftPoly.variableList;
	std::list<Var>& right = rightPoly.variableList;

	// (1)
	for (auto& left_e : left)
	{
		auto elem = std::find_if(right.begin(), right.end(),
			[&left_e](const Var &x)->bool { return x.letter == left_e.letter; });

		// found
		if (elem != right.end())
		{
			left_e.power += elem->power;
			right.erase(elem);
		}
	}

	// found a zero degree -> 1 -> remove from list letter
	left.remove_if([](const Var& x) { return x.power == 0; });

	// (2, 3)
	left.insert(left.end(), right.begin(), right.end());

	// ab != ba -> sort
	left.sort();
	return leftPoly;
}

const bool operator == (const PolyElem &left, const PolyElem &right)
{
	return left.variableList == right.variableList;
}

const bool operator > (const PolyElem &left, const PolyElem &right)
{
	if (left.variableList.size() < right.variableList.size())
	{
		if (left.variableList < right.variableList)
			return true;
		else
			return false;
	}
		
	else 
		return false;
}

#pragma endregion POLYELEM

#pragma region POLYNOMIAL

Polynomial::Polynomial(const double& ival, const char& cval, const double& ipow)
{
	PolyElem e;
	e.coefficient = ival;

	if (cval != 0 && ival != 0)
	{
		Var var;
		var.letter = cval;
		var.power = ipow;

		e.variableList.push_back(var);
	}

	listPoly.push_back(e);
}

void Polynomial::assignName(const std::string& x)
{
	assert(this->name.empty(), "Fatal: name to variable has been already assigned");
	name = x;
}

const std::string& Polynomial::getName()
{
	return name;
}

std::list<PolyElem>& Polynomial::getValue()
{
	return this->listPoly;
}

Polynomial Polynomial::operator= (const Polynomial &e)
{
	this->listPoly = e.listPoly;
	this->name = e.name;
	return *this;
}

std::string Polynomial::out(bool in_column)
{
	if (this->listPoly.empty())
		return "Not initialized";

	std::string nl = in_column ? "\n" : "";

	listPoly.sort([](const PolyElem &a, const PolyElem &b)->bool { return a > b; });
	//listPoly.reverse();

	auto iter = listPoly.begin();
	auto iend = listPoly.end();
	std::string s = iter->out(true);

	for (++iter; iter != iend; iter++)
	{
		if (iter->coefficient >= 0)
			s += " + " + nl + iter->out();
		else		   
			s += " - " + nl + iter->out();
	}
		

	return s;
}

Polynomial Polynomial::operator- ()
{
	Polynomial newPoly = *this;
	std::list<PolyElem>& newPolyList = newPoly.listPoly;

	std::for_each(newPolyList.begin(), newPolyList.end(), [](PolyElem& x) {x.coefficient *= -1; });
	return newPoly;
}

Polynomial operator+ (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	Polynomial newPoly = leftPoly;
	return newPoly += rightPoly;
}

/*
The addition of two polynomials
Algorithm:
1) For each element in the list right find the left element, which variableList is the same
2 ) - if the element is found, then add the integer coefficients of the elements
	- Otherwise, attach the item from the right list to the left list
*/
Polynomial operator+= (Polynomial& leftPoly, const Polynomial& rightPoly)
{
	auto& left = leftPoly.listPoly;
	auto& right = rightPoly.listPoly;

	auto left_begin = left.begin();
	auto left_end = left.end();

	// (1)
	for (auto& right_e : right)
	{
		auto elem = std::find(left_begin, left_end, right_e);

		// (2)
		if (elem != left_end)
			elem->coefficient += right_e.coefficient;
		else
			left.insert(left.cend(), right_e);
	}

	// remove elements with zero coefficients
	left.remove_if([](const PolyElem& x) { return x.coefficient == 0; });

	// got 0
	if (left.empty())
	{
		PolyElem x;
		x.coefficient = 0;
		left.push_back(x);
	}

	return leftPoly;
}

Polynomial operator- (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	Polynomial newPoly = leftPoly;
	return newPoly -= rightPoly;
}

Polynomial operator-= (Polynomial& leftPoly, const Polynomial& rightPoly) 
{
	Polynomial x = rightPoly;
	return leftPoly += -x;
}

/*
Multiplication of two polynomials
Algorithm: (a1 .. aN) (b1 .. bN) - > multiply all elements and add
*/
Polynomial operator* (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	// check 0 * () è () * 0
	if (leftPoly.listPoly.size() == 1 && leftPoly.listPoly.front().coefficient == 0
		|| rightPoly.listPoly.size() == 1 && rightPoly.listPoly.front().coefficient == 0)
	{
		return Polynomial(0);
	}

	Polynomial newPoly(0);
	auto& left = leftPoly.listPoly;
	auto& right = rightPoly.listPoly;

	for (auto& elem_l : left)
	{
		for (auto& elem_r : right)
		{
			newPoly += Polynomial(elem_l * elem_r);
		}
	}

	return newPoly;
}

/*
Exponentiation: (x1 + ... + xm) ^ n
1) https://ru.wikipedia.org/wiki/%D0%9C%D1%83%D0%BB%D1%8C%D1%82%D0%B8%D0%BD%D0%BE%D0%BC%D0%B8%D0%B0%D0%BB%D1%8C%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D1%8D%D1%84%D1%84%D0%B8%D1%86%D0%B8%D0%B5%D0%BD%D1%82
2) https://habrahabr.ru/post/153255/
*/
Polynomial operator^ (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	const std::list<PolyElem>& right = rightPoly.listPoly;
	assert(right.size() == 1, "Semantic error: degree must be a single element");

	if (leftPoly.listPoly.size() == 1)
		return Polynomial(leftPoly.listPoly.front() ^ right.front());
	else
	{
		const char* s = "Semantic error: in operation '(Polynomial) ^ n' n must be a non-negative integer";
		const PolyElem& power = right.front();

		// rightPoly is a number
		assert(power.variableList.size() == 0, s);

		// rightPoly is an integer number
		assert(power.coefficient == (double)(int)power.coefficient, s);

		// rightPoly - is a non-negative integer number
		assert(power.coefficient >= 0, s);

		const unsigned n = (const unsigned)power.coefficient;

		if (n == 0)
			return Polynomial(1);
		else if (n == 1)
			return leftPoly;
		else
		{
			Polynomial result = Polynomial(1);
			Polynomial copy = leftPoly;

			for (unsigned i = 0; i < n; i++)
				result = result * copy;

			return result;
		}
	}
}

#pragma endregion POLYNOMIAL
