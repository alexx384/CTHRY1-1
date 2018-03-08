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

// Структура для элемента полинома: 1 2x, -x, x^2, 5x^7, 45abc^3 и т. д.
// coefficient - целочисленный коэффициент
// variableList - переменные (буквы), у каждой своя степень
struct PolyElem
{
	double coefficient;
	std::list<Var> variableList;
	
	// Обнулить элемент
	PolyElem setToZero();

	// вывод элементов в строку
	std::string out(bool first = false);

	PolyElem operator= (const PolyElem &e);

	friend PolyElem operator^ (const PolyElem& leftPoly, const PolyElem& rightPoly);

	/*Возведение элемента полинома в степень, например, (11ab^2c^2)^(-2.08)
	Алгоритм:
	1) возвести в степень power = rightPoly.coefficient целочисленный коэффициент
	2) для каждой переменной умножить её текущую степень на степень power
	Если его степень стала равной 0, то удалить элемент из списка
	*/
	friend PolyElem operator^= (PolyElem& leftPoly, const PolyElem& rightPoly);

	friend PolyElem operator* (const PolyElem& leftPoly, PolyElem rightPoly);

	/*Умножение элементов в полиноме, таких как 111 abc 111abc 11ab^2c^2
	Алгоритм:
	1) Перемножить коэф. полиномов, затем поэлементно в списке left:
	1.1) если нашли такую же букву в right, то увеличиваем степень элемента
	в left на степень элемента в right и удаляем тот элемент from right

	2) Если список right не пустой, то присоединить элементы из right к left
	3) Возврат left (newPoly)

	*/
	friend PolyElem operator*= (PolyElem& leftPoly, PolyElem rightPoly);
};

// Неполное совпадение ==
// Для полного совпадения использовать
// left.variableList == right.variableList && left.coef == right.coef
const bool operator == (const PolyElem &left, const PolyElem &right);

const bool operator > (const PolyElem &left, const PolyElem &right);

// Структура для полинома: 1+2x-x^2+5x^7-45abc^3 и т. д.
class Polynomial
{
	// список хранит слагаемые 1 2x x^2 ...
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

	// Вывод полинома в строку
	std::string out(bool in_column = false);

	// унарный минус: -(polynomial)
	Polynomial operator- ();

	/*
	Сложение двух полиномов
	Алгоритм:
	1) Для каждого элемента в списке right ищем в списке left элемент, у которого variableList такой же
	2)	- Если элемент нашелся, то складываем целочисленные коэффициенты элементов
		- Иначе присоединяем элемент из списка right к списку left
	*/
	friend Polynomial operator+ (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator+= (Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator- (const Polynomial& leftPoly, const Polynomial& rightPoly);

	friend Polynomial operator-= (Polynomial& leftPoly, const Polynomial& rightPoly);

	/*
	Умножение двух полиномов
	Алгоритм: (a1 .. aN) (b1 .. bN) -> перемножаем все элементы и складываем
	*/
	friend Polynomial operator* (const Polynomial& leftPoly, const Polynomial& rightPoly);

	/*
	Возведение в степень: (x1 + ... + xm) ^ n
	Алгоритм: 
	1) https://ru.wikipedia.org/wiki/%D0%9C%D1%83%D0%BB%D1%8C%D1%82%D0%B8%D0%BD%D0%BE%D0%BC%D0%B8%D0%B0%D0%BB%D1%8C%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D1%8D%D1%84%D1%84%D0%B8%D1%86%D0%B8%D0%B5%D0%BD%D1%82
	2) https://habrahabr.ru/post/153255/
	*/
	friend Polynomial operator^ (const Polynomial& leftPoly, const Polynomial& rightPoly);
};

// основная функция для вычисления
Polynomial calculate(Polynomial a, Polynomial b, char op);
