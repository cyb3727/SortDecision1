#pragma once
#include "LinearFunction.h"
#include "UnitTest.h"
#include <vector>

class SortDecision
{
private:
	char decision_file_path[256];
	char test_file_path[256];

	void save(LinearFunction& LFr, LinearFunction& LFw);
	void load(LinearFunction& LFr, LinearFunction& LFw);
	LinearFunction* CalcFw(char* file_path, size_t M, size_t B);
	LinearFunction* CalcFr(char* file_path, size_t M, size_t B);
	tuple<double, size_t, size_t> Trisection(size_t N, size_t M, size_t B, size_t k, LinearFunction* Fr, LinearFunction* Fw);
	double TCost(size_t N, size_t M, double Br, size_t k, LinearFunction* Fr, LinearFunction* Fw);
	double NormalCost(size_t N, size_t M, size_t B, size_t output_size, LinearFunction* Fr, LinearFunction* Fw);
	double TraditionalCost(size_t N, size_t M, size_t B, LinearFunction* Fr, LinearFunction* Fw);
	bool exist(char* path);
	size_t myceil(double x);
	int createfile(char* file_path, size_t size);
	int removefile(char* file_path);

public:

	/**
	 * give the setting of ssd path
	 * parameter decision_file_path: the result file saved
	 * parameter test_file_path: the position of testing file to be saved
	 */
	SortDecision(const char* decision_file_path, const char* test_file_path);

	/**
	 * Return the optimial setting of MergeSort
	 * parameter N: the volume of data which is to be sorted
	 * parameter M: the internal memory used for sorting
	 * parameter M_max: the Max memory of computer to
	 * paramter B: the read or write unit
	 */
	tuple<double, size_t, size_t, size_t> Decide(size_t N, size_t M, size_t M_max, size_t B);

	/**
	 * Return the cost of common settings
	 * parameter N: the volume of data which is to be sorted
	 * parameter M: the internal memory used for sorting
	 * paramter B: the read or write unit
	 */
	double RetNormalCost(size_t N, size_t M, size_t B, size_t output_size);

	double RetTraditionalCost(size_t N, size_t M, size_t B);

	void GetOnlyArchive(size_t M_max, size_t B);
};

