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

// ��������� ��� �������� ��������: 1 2x, -x, x^2, 5x^7, 45abc^3 � �. �.
// coefficient - ������������� �����������
// variableList - ���������� (�����), � ������ ���� �������
struct PolyElem
{
	double coefficient;
	std::list<Var> variableList;
	
	// �������� �������
	PolyElem setToZero();

	// ����� ��������� � ������
	std::string out(bool first = false);

	PolyElem operator= (const PolyElem &e);

	friend PolyElem operator^ (const PolyElem& leftPoly, const PolyElem& rightPoly);

	/*���������� �������� �������� � �������, ��������, (11ab^2c^2)^(-2.08)
	��������:
	1) �������� � ������� power = rightPoly.coefficient ������������� �����������
	2) ��� ������ ���������� �������� � ������� ������� �� ������� power
	���� ��� ������� ����� ������ 0, �� ������� ������� �� ������
	*/
	friend PolyElem operator^= (PolyElem& leftPoly, const PolyElem& rightPoly);

	friend PolyElem operator* (const PolyElem& leftPoly, PolyElem rightPoly);

	/*��������� ��������� � ��������, ����� ��� 111 abc 111abc 11ab^2c^2
	��������:
	1) ����������� ����. ���������, ����� ����������� � ������ left:
	1.1) ���� ����� ����� �� ����� � right, �� ����������� ������� ��������
	� left �� ������� �������� � right � ������� ��� ������� from right

	2) ���� ������ right �� ������, �� ������������ �������� �� right � left
	3) ������� left (newPoly)

	*/
	friend PolyElem operator*= (PolyElem& leftPoly, PolyElem rightPoly);
};

// �������� ���������� ==
// ��� ������� ���������� ������������
// left.variableList == right.variableList && left.coef == right.coef
const bool operator == (const PolyElem &left, const PolyElem &right);

const bool operator > (const PolyElem &left, const PolyElem &right);

// ��������� ��� ��������: 1+2x-x^2+5x^7-45abc^3 � �. �.
class Polynomial
{
	// ������ ������ ��������� 1 2x x^2 ...
	std::list<PolyElem> listPoly;
	std::string name;

	Polynomial(const PolyElem &x)
	{
		this->listPoly.push_back(x);
	}

public:

	// ax^n int ival - a, int cval - x, int ipow - n
	Polynomial(const double& ival = 0, const char& cval = 0, const double& ipow = 1);

	void assignName(const char* x);

	const char* getName();// {if (name.empty()) throw std::exception() }

	Polynomial operator= (const Polynomial &e);

	// ����� �������� � ������
	std::string out(bool in_column = false);

	// ������� �����: -(polynomial)
	Polynomial operator- ();

	/*
	�������� ���� ���������
	��������:
	1) ��� ������� �������� � ������ right ���� � ������ left �������, � �������� variableList ����� ��
	2)	- ���� ������� �������, �� ���������� ������������� ������������ ���������
		- ����� ������������ ������� �� ������ right � ������ left
	*/
	friend Polynomial operator+ (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator+= (Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator- (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator-= (Polynomial& leftPoly, const Polynomial& rightPoly);

	/*
	��������� ���� ���������
	��������: (a1 .. aN) (b1 .. bN) -> ����������� ��� �������� � ����������
	*/
	friend Polynomial operator* (const Polynomial& leftPoly, const Polynomial& rightPoly);

	/*
	���������� � �������: (x1 + ... + xm) ^ n
	��������: 
	1) https://ru.wikipedia.org/wiki/%D0%9C%D1%83%D0%BB%D1%8C%D1%82%D0%B8%D0%BD%D0%BE%D0%BC%D0%B8%D0%B0%D0%BB%D1%8C%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D1%8D%D1%84%D1%84%D0%B8%D1%86%D0%B8%D0%B5%D0%BD%D1%82
	2) https://habrahabr.ru/post/153255/
	*/
	friend Polynomial operator^ (const Polynomial& leftPoly, const Polynomial& rightPoly);
};

// �������� ������� ��� ����������
Polynomial calculate(Polynomial a, Polynomial b, char op);
