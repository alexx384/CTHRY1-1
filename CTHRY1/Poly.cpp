#include "Poly.h"
#include "Multinomial.h"
#include <algorithm>

#ifdef _DEBUG
#define assert(x, s)											\
if (!(x)) throw std::exception(('\'' + std::string(s) + '\''	\
+ "\nFUNCTION: " + std::string(__FUNCTION__)					\
+ "\nFILE: "	+ std::string(__FILE__) +  						\
+ "\nLINE: " + std::to_string(__LINE__)).c_str())		

#else

#define assert(x,s) if (!(x))	\
throw std::exception(('\'' + std::string(s) + '\'').c_str())

#endif // DEBUG


std::string doubleToString(double x)
{
	char buf[56] = { 0 };
	sprintf_s(buf, sizeof(buf), "%g", x);

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
		assert(elem.letter != 0, "elem.letter was null at output");
		assert(elem.power != 0, "elem.power was null at output");

		if (elem.power == 1)
			s += "*" + std::string(1, elem.letter);
		else if (elem.power > 1)
			s += "*" + std::string(1, elem.letter) + "^" + doubleToString(elem.power);
		else
			s += "*" + std::string(1, elem.letter) + "^(" + doubleToString(elem.power) + ')';
	}

	// убрать: 1*x -> х
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

/*Возведение элемента полинома в степень, например, (11ab^2c^2)^(-2.08)
Алгоритм:
1) возвести в степень power = rightPoly.coefficient целочисленный коэффициент
2) для каждой переменной умножить её текущую степень на степень power
Если его степень стала равной 0, то удалить элемент из списка
*/
PolyElem operator^= (PolyElem& leftPoly, const PolyElem& rightPoly)
{
	assert(rightPoly.variableList.empty(), "Operation 'x ^ n' where n is not number is not defined");
	assert(leftPoly.coefficient != 0, "Operation '0 ^ n' has undefined behaviour");

	std::list<Var>& newPolyList = leftPoly.variableList;

	// (1)
	double power = rightPoly.coefficient;
	leftPoly.coefficient = pow(leftPoly.coefficient, power);

	// (2)
	for (auto& elem : newPolyList)
		elem.power *= power;

	// нашли нулевую степень -> 1 -> убираем из списка букву
	newPolyList.remove_if([](const Var& x) { return x.power == 0; });
	return leftPoly;
}

PolyElem operator* (const PolyElem& leftPoly, PolyElem rightPoly)
{
	PolyElem newPoly = leftPoly;
	return newPoly *= rightPoly;
}

/*Умножение элементов в полиноме, таких как 111 abc 111abc 11ab^2c^2
Алгоритм:
1) Перемножить коэф. полиномов, затем поэлементно в списке left:
1.1) если нашли такую же букву в right, то увеличиваем степень элемента
в left на степень элемента в right и удаляем тот элемент from right

2) Если список right не пустой, то присоединить элементы из right к left
3) Возврат left (newPoly)

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

		// нашли
		if (elem != right.end())
		{
			left_e.power += elem->power;
			right.erase(elem);
		}
	}

	// нашли нулевую степень -> 1 -> убираем из списка букву
	left.remove_if([](const Var& x) { return x.power == 0; });

	// (2, 3)
	left.insert(left.end(), right.begin(), right.end());
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

void Polynomial::assignName(const char* x)
{
	assert(this->name.empty(), "Name to variable has been already assigned");
	name = x;
}

const char* Polynomial::getName()
{
	assert(!name.empty(), "No name was assigned to variable");
	return name.c_str();
}

Polynomial Polynomial::operator= (const Polynomial &e)
{
	this->listPoly = e.listPoly;
	return *this;
}

std::string Polynomial::out(bool in_column)
{
	std::string nl = in_column ? "\n" : "";

	listPoly.sort([](const PolyElem &a, const PolyElem &b)->bool { return a > b; });
	listPoly.reverse();

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
Сложение двух полиномов
Алгоритм:
1) Для каждого элемента в списке right ищем в списке left элемент, у которого variableList такой же
2)	- Если элемент нашелся, то складываем целочисленные коэффициенты элементов
- Иначе присоединяем элемент из списка right к списку left
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

	// убрать элементы с нулевыми коэф - ми
	left.remove_if([](const PolyElem& x) { return x.coefficient == 0; });

	// получился 0
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
Умножение двух полиномов
Алгоритм: (a1 .. aN) (b1 .. bN) -> перемножаем все элементы и складываем
*/
Polynomial operator* (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	// проверка 0 * () и () * 0
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
Возведение в степень: (x1 + ... + xm) ^ n
1) https://ru.wikipedia.org/wiki/%D0%9C%D1%83%D0%BB%D1%8C%D1%82%D0%B8%D0%BD%D0%BE%D0%BC%D0%B8%D0%B0%D0%BB%D1%8C%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D1%8D%D1%84%D1%84%D0%B8%D1%86%D0%B8%D0%B5%D0%BD%D1%82
2) https://habrahabr.ru/post/153255/
*/
Polynomial operator^ (const Polynomial& leftPoly, const Polynomial& rightPoly)
{
	const std::list<PolyElem>& right = rightPoly.listPoly;
	assert(right.size() == 1, "Degree must be a single element");

	if (leftPoly.listPoly.size() == 1)
		return Polynomial(leftPoly.listPoly.front() ^ right.front());
	else
	{
		const char*s = "In operation '(Polynomial) ^ n' n must be a non-negative integer";
		const PolyElem& power = right.front();

		// rightPoly - это число
		assert(power.variableList.size() == 0, s);

		// rightPoly - это целое число
		assert(power.coefficient == (double)(int)power.coefficient, s);

		// rightPoly - это целое неотрицательное число
		assert(power.coefficient >= 0, s);

		const unsigned n = (const unsigned)power.coefficient;
		const unsigned m = (const unsigned)leftPoly.listPoly.size();

		if (n == 0)
			return Polynomial(1);
		else if (n == 1)
			return leftPoly;
		else
		{
			Polynomial result(0);
			NMK_FIND nmk = NMK_FIND(m, n);

			auto& k = nmk.getArrays();
			for (const auto& arr : k)
			{
				PolyElem sum, pow;
				sum.coefficient = (double)MultiNomial::BinomAr(arr.data(), m);

				// m = listPoly.size()
				auto iter = leftPoly.listPoly.begin();

				for (unsigned i = 0; i < m; i++)
				{
					pow.coefficient = arr[i];
					sum *= (*iter ^ pow);
					iter++;
				}

				result += Polynomial(sum);
			}
			return result;
		}
	}
}

#pragma endregion POLYNOMIAL

Polynomial calculate(Polynomial a, Polynomial b, char op)
{
	Polynomial c;

	try
	{
		assert(op == '+' || op == '-' || op == '*' || op == '^', "Unknown operation");

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

	return c;
}