#pragma once
#include <cstdio>

template<typename kv>
struct losenode {
	size_t pos;
	bool valid;
	kv key_value;
	losenode() {
	}

	losenode(size_t pos, kv key_value, bool valid) {
		set(pos, key_value, valid);
	}

	void set(size_t pos, kv key_value, bool valid)
	{
		this->pos = pos;
		memmove(&this->key_value, &key_value, sizeof(kv));
		this->valid = valid;
	}

	void setinvalid(size_t pos)
	{
		this->pos = pos;
		this->valid = false;
		memset(&key_value, -1, sizeof(kv));
	}

	bool operator<(const losenode& a)const
	{
		if (this->valid == false) {
			return false;
		}
		else if (a.valid == false)
		{
			return true;
		}
		else {
			return this->key_value < a.key_value;
		}
	}
};

template<typename kv>
static bool isvalid(losenode<kv>* n) {
	return n->valid;
}