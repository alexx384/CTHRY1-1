#include "Multinomial.h"

ulong MultiNomial::BinomAr(const uint *numbers, uint Length)
{
	if (Length == 1)
		return 1;

	ulong result = 1;
	uint sum = numbers[0];

	for (uint i = 1; i < Length; i++)
	{
		sum += numbers[i];
		result *= Binominal(sum, numbers[i]);
	}

	return result;
}

ulong MultiNomial::Binominal(ulong n, ulong k)
{
	ulong r = 1;
	ulong d;
	if (k > n) return 0;
	for (d = 1; d <= k; d++)
	{
		r *= n--;
		r /= d;
	}
	return r;
}


NMK_FIND::NMK_FIND(uint sum_elements_cnt, uint sum)
{
	Array = std::vector<uint>(sum_elements_cnt);
	sumToGet = sum;
	N = sum_elements_cnt;

	frec();
}

const std::vector<std::vector<uint>>& NMK_FIND::getArrays()
{
	return pArray;
}

uint NMK_FIND::sum(std::vector<uint> v)
{
	uint s = 0;
	for (uint& e : v)
		s += e;
	return s;
}

void NMK_FIND::frec(uint i)
{
	uint s = sum(Array);
	if (s == sumToGet)
		pArray.push_back(Array);

	if (i == N)
		return;

	for (uint k = 0; k <= sumToGet; k++)
	{
		Array[i] = k;
		frec(i + 1);
	}
}
