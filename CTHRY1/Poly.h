#pragma once

#include <list>
#include <string>
#include <vld.h>

struct Var
{
	char letter;
	double power;
};

const bool operator == (const Var &left, const Var &right);

const bool operator < (const Var &left, const Var &right);

// Structure of polynomial element such as: 1 2x, -x, x^2, 5x^7, 45abc^3 è ò. ä.
// coefficient - double value
// variableList - variables (letters), each has its own degree
struct PolyElem
{
	double coefficient;
	std::list<Var> variableList;
	
	// To zero the element
	PolyElem setToZero();

	// the output of the elements to the string
	std::string out(bool first = false);

	PolyElem operator= (const PolyElem &e);

	friend PolyElem operator^ (const PolyElem& leftPoly, const PolyElem& rightPoly);

	//	Exponentiation of polynomial element, for example(11a b ^ 2c ^ 2) ^ (-2.08)
	friend PolyElem operator^= (PolyElem& leftPoly, const PolyElem& rightPoly);

	friend PolyElem operator* (const PolyElem& leftPoly, PolyElem rightPoly);

	// Multiplication of elements in a polynomial such as 111abc111a b c 11 a b^2c^2
	friend PolyElem operator*= (PolyElem& leftPoly, PolyElem rightPoly);
};

// Partial match ==
// For full match, use
// left.variable List == right.variableList && left.coef = = right.coef

const bool operator == (const PolyElem &left, const PolyElem &right);

const bool operator > (const PolyElem &left, const PolyElem &right);

// Structure for polynomial: 1+2x-x^2+5x^7-45 abc^3, etc.
class Polynomial
{
	// the list stores the summands 1 2x x^2 ...
	std::list<PolyElem> listPoly;

	// it's name
	std::string name;

	Polynomial(const PolyElem &x)
	{
		this->listPoly.push_back(x);
	}

public:

	// ax^n ival - a, cval - x, ipow - n
	Polynomial(const double& ival = 0, const char& cval = 0, const double& ipow = 1);

	// assign name to polynomial (need for variables)
	void assignName(const char* x);

	// get name of polynomial (need for variables)
	const char* getName();

	Polynomial operator= (const Polynomial &e);

	// The output of the polynomial to a string
	std::string out(bool in_column = false);

	// unary minus: -(polynomial)
	Polynomial operator- ();

	// The addition of two polynomials
	friend Polynomial operator+ (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator+= (Polynomial& leftPoly, const Polynomial& rightPoly);

	// The substraction of two polynomials
	friend Polynomial operator- (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator-= (Polynomial& leftPoly, const Polynomial& rightPoly);

	// The multiplication of two polynomials
	friend Polynomial operator* (const Polynomial& leftPoly, const Polynomial& rightPoly);

	//Polyomial Exponentiation: (x1 + ... + xm) ^ n
	friend Polynomial operator^ (const Polynomial& leftPoly, const Polynomial& rightPoly);
};

// basic function for calculation
Polynomial calculate(Polynomial a, Polynomial b, char op);
