#include "run.h"

run::run(data* sort_data, size_t block_s, size_t block_e)
{
	this->sort_data = sort_data;
	this->block_s = block_s;
	this->block_e = block_e;
	this->cur_block = this->block_s;
}

bool run::endofrun()
{
	return this->cur_block > this->block_e;
}

void run::readseqblock(block* b, char* Br, size_t Br_size)
{
	size_t size = this->sort_data->readbuf(Br, this->cur_block * Br_size, Br_size);
	b->set(Br, 0, size - 1, this->cur_block);
	this->cur_block += 1;
}

size_t run::get_s()
{
	return this->block_s;
}

size_t run::get_e()
{
	return this->block_e;
}
