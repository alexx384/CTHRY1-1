#pragma once

#include <vector>
typedef unsigned long ulong;
typedef unsigned int uint;

class NMK_FIND
{
	uint sumToGet;
	uint N;

	std::vector<std::vector<uint>> pArray;
	std::vector<uint> Array;

	// найти сумму элементов v
	uint sum(std::vector<uint> v);

	// полный рекурсивный перебор значений от 0 до N
	// Если сумма k1 + ... + kn == sumToGet, то сохранить этот набор
	void frec(uint i = 0);

public:

	/*
	Найти все числа s[i], количество которых sum_elements_cnt,
	такие что сумма _sum(s[i]) = sum
	*/
	NMK_FIND(uint sum_elements_cnt, uint sum);

	// Возврат всех наборов длины n чисел s[i], для которых сумма _sum(s[i]) = sum
	const std::vector<std::vector<uint>>& getArrays();
};

class MultiNomial
{

public:

	// найти мультиномиальный коэффициент путем перемножения биномиальных коэффициентов
	static ulong BinomAr(const uint *numbers, uint Length);

	// найти биномиальный коэффициент С(n, k)
	static ulong Binominal(ulong n, ulong k);
};
