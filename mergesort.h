#pragma once
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstring>
#include "run.h"
#include "data.h"
#include "losenode.h"

class mergesort
{
	size_t k;
	std::vector<char*> Brs;
	char* Bw;
	data* sort_data;
	char* mem;
public:
	mergesort(size_t k, size_t M, size_t Br, size_t Bw);

	/**
	 * data_file: the data to be sorted
	 * M: the memory
	 * rw_unit: the read/write unit
	 * kv_len: kv length
	 * k: k-way merge sorting
	 * file_res: the data file to save sorted result
	 */
	bool merge_run(char* data_file, size_t M, int rw_unit, int kv_len, size_t k, char* file_res);

	/**
	 * data_file: the source data
	 * rw_unit: the read/write unit
	 * kv_unit: the kv unit
	 * runs: the formation runs
	 * cmp: the comparing function
	 */
	void format_run(char* data_file, size_t M, int rw_unit, int kv_unit, std::vector<run*>* runs, int (*cmp)(const void*, const void*));

	/**
	 * parameter runs: the runs to be merged
	 * parameter run: the merged run
	 * parameter target_data: the data to be saved
	 */
	void merge_k_run(std::vector<run*>* runs, run* run, data* target_data);

	~mergesort();
};

