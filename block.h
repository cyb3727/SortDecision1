#pragma once
#include <cstdio>
#include <iostream>
#include <cstring>

struct block
{
	size_t blockId;
	size_t s;
	size_t e;
	size_t c;
	char* buf;

	block() {
	};

	block(char* buf, size_t s, size_t e, size_t blockId) {
		set(buf, s, e, blockId);
	}

	void set(char* buf, size_t s, size_t e, size_t blockId)
	{
		this->buf = buf;
		this->s = s;
		this->e = e;
		this->blockId = blockId;
		this->c = s;
	}

	bool getkv(void* kv, size_t size)
	{
		if (c + size - 1 <= e) {
			memmove(kv, buf + c, size);
			c += size;
			//printf("c is: %d\n", c);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool end() {
		return c > e;
	}
};

