#pragma once
#include <cstdio>
#include "data.h"
#include "block.h"
class data;

class run
{
private:
	data* sort_data;
	size_t cur_block;
	size_t block_s;
	size_t block_e;
public:
	run(data* sort_data, size_t block_s, size_t block_e);
	bool endofrun();

	/**
	 * b: block information to be stored
	 * Br: the buf used to store data
	 * Br_size: buf size of Br
	 */
	void readseqblock(block* b, char* Br, size_t Br_size);
	size_t get_s();
	size_t get_e();
};

