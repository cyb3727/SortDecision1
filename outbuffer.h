#pragma once
#include <cstdio>
#include "data.h"

struct outbuffer {
	data* target_data;
	size_t buf_size;
	size_t cur;
	char* buf;
	outbuffer() {
		this->cur = 0;
	}

	void set(char* buf, size_t buf_size, data* target_data) {
		this->target_data = target_data;
		this->buf = buf;
		this->buf_size = buf_size;
	}

	void append(void* kv, size_t kv_size) {
		if (cur + kv_size > buf_size)
		{
			flush();
		}

		memmove(buf + cur, kv, kv_size);
		//printf("append: %lld\n", *((long*)kv));
		cur += kv_size;
	}

	void flush()
	{
		target_data->append(buf, this->cur);
		this->cur = 0;
	}

	void close() {
		flush();
	}
};