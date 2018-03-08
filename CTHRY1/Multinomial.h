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

	// ����� ����� ��������� v
	uint sum(std::vector<uint> v);

	// ������ ����������� ������� �������� �� 0 �� N
	// ���� ����� k1 + ... + kn == sumToGet, �� ��������� ���� �����
	void frec(uint i = 0);

public:

	/*
	����� ��� ����� s[i], ���������� ������� sum_elements_cnt,
	����� ��� ����� _sum(s[i]) = sum
	*/
	NMK_FIND(uint sum_elements_cnt, uint sum);

	// ������� ���� ������� ����� n ����� s[i], ��� ������� ����� _sum(s[i]) = sum
	const std::vector<std::vector<uint>>& getArrays();
};

class MultiNomial
{

public:

	// ����� ���������������� ����������� ����� ������������ ������������ �������������
	static ulong BinomAr(const uint *numbers, uint Length);

	// ����� ������������ ����������� �(n, k)
	static ulong Binominal(ulong n, ulong k);
};
