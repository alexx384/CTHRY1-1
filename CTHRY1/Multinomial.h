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

	// get sum of elements in vector v
	uint sum(std::vector<uint> v);

	// full recursive iteration of values from 0 to N
	// If the sum is k1+... + kn == sumToGet then to keep this vector
	void frec(uint i = 0);

public:

	/*
	Find all numbers s[i], the number of which sum_elements_cnt,
	such that the sum _sum(s[i]) = sum
	*/
	NMK_FIND(uint sum_elements_cnt, uint sum);

	// Return all sets of length n of numbers s[i] for which sum _sum (s[i]) = sum
	const std::vector<std::vector<uint>>& getArrays();
};

class MultiNomial
{

public:

	// find the polynomial coefficient by multiplying the binomial coefficients
	static ulong BinomAr(const uint *numbers, uint Length);

	// find the binomial coefficient C(n, k)
	static ulong Binominal(ulong n, ulong k);
};
