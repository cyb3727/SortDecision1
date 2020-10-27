#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <cstdio>
#include <sys/stat.h>
#include "run.h"
class run;

class data
{
	int fd;
	size_t data_size;
	size_t cur_pos;
	char file_path[256];

	bool checkallignment(char* buf);
	size_t getfilesize(const char* file_name);
public:
	data(char* file_path, int mode);
	size_t getsize();
	void getfilepath(char* file_path);
	size_t readbuf(char* buf, size_t off, size_t len);
	size_t writebuf(char* buf, size_t off, size_t len);
	size_t append(char* buf, size_t size);
	void clear();
	size_t readblock(char* buf, size_t blockId);
	run* createrun(size_t block_s, size_t block_e);
	std::vector<run*>* split_run(size_t M, size_t rw_unit);
};

