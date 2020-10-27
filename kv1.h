#pragma once
#include <fstream> 

struct kv1 {
	long key;
	long value;
	bool operator<(const kv1& a)const
	{
		return key < a.key;
	}

	bool operator==(const kv1& a)const
	{
		return key == a.key;
	}

	bool operator>(const kv1& a)const
	{
		return key > a.key;
	}

	friend std::ostream& operator << (std::ostream& out, kv1& d)
	{
		std::cout << "{" << d.key << "," << d.value << "}";
	}
};

static int compare(const void* arg1, const void* arg2) {
	kv1* a1 = (kv1*)arg1;
	kv1* a2 = (kv1*)arg2;
	return a1->key > a2->key;
}