#pragma once
#include <iostream>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include "losertree.h"
#include "BlockManager.h"
#include "Randomer.h"
#include "Timer.h"
#include "Process.h"
#include "template_mergesort.h"
#include "kv1.h"

class UnitTest
{
private:
	size_t GetFileSize(const char* file_name) {
		struct stat statbuf;
		stat(file_name, &statbuf);
		int size = statbuf.st_size;
		return size;
	}
public:
	void GenerateLongSortData(char* file_path, ulong file_size, int block_size);

	/**
	* test the rand read IOPS and bandwidth to compare it to fio test
	* parameter file_size: testing file size
	* parameter block_size: the unit block to be read once
	* parameter file_name: the name of file will be created
	* parameter bandwidth: the bandwidth detected
	* return: FAIL or SUCCESS
	*/
	int TestBlockManagerRandReadSpeed(int block_size, const char* file_name, double& bandwidth, int seconds);

	/**
	* test the rand write IOPS and bandwidth to compare it to fio test
	* parameter file_size: testing file size
	* parameter block_size: the unit block to write once
	* parameter file_name: the name of file will be created
	* parameter bandwidth: the bandwidth detected
	* return: FAIL or SUCCESS
	*/
	int TestBlockManagerRandWriteSpeed(int block_size, const char* file_name, double& bandwidth, int seconds);

	/**
	 * test the algorithm
	 */
	void TestMergeSort();


	//void TestTraditionalMergeSort(char* src, char* des, size_t M, size_t N);

	struct node {
		int pos;
		char* kv[8];
		bool valid;//invalid is equal to infity
		node() {}

		node(int pos, long key, long value, bool valid = true) {
			this->pos = pos;
			*((long*)kv) = key;
			*((long*)(kv + 4)) = value;
			this->valid = valid;
		}

		bool operator<(const node& a)const
		{
			if (this->valid == false) {
				return false;
			}
			else if (a.valid == false)
			{
				return true;
			}
			else {
				long key = long(*kv);
				long key1 = long(*a.kv);
				return key < key1;
			}
		}
	};
	static bool isvalid(node* n)
	{
		return n->valid;
	}

	void TestLoserTree();
};

