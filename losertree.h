#pragma once
#include "run.h"

template<typename T>
class losertree
{
	size_t depth;
	size_t n;
	T** t;
	bool (*isvalid)(T*);//which is used to judge whether the node has invalid data yet
public:
	losertree(std::vector<T>* v, bool (*isvalid)(T*));
	void build(std::vector<T>* v);
	size_t calculate_depth(size_t n);

	//return false means that it is the end of losertree
	bool get_min(T* res);
	void set_value(size_t pos, T* value);
	void adjust(size_t pos);
	~losertree();
};

template<typename T>
inline losertree<T>::losertree(std::vector<T>* v, bool (*isvalid)(T*))
{
	this->isvalid = isvalid;
	build(v);
}

template<typename T>
inline void losertree<T>::build(std::vector<T>* v)
{
	n = v->size();
	depth = calculate_depth(n);
	t = new T * [depth];
	size_t all = 1 << (depth - 1);
	for (size_t i = 0; i < depth; i++)
	{
		t[i] = new T[1 << i];
	}

	//init the leaf node data
	memset(t[depth - 1], -1, all * sizeof(T));
	for (size_t i = 0; i < n; i++) {
		t[depth - 1][i] = (*v)[i];
	}

	size_t child_num = n;
	//update the loser from bottom to up
	for (int i = depth - 2; i >= 0; i--)
	{
		size_t father_num = child_num / 2 + child_num % 2;
		for (int j = 0; j < father_num; j++)
		{
			size_t left_child = 2 * j;
			size_t right_child = 2 * j + 1;
			t[i][j] = t[i + 1][left_child];
			if (right_child < child_num && t[i + 1][right_child] < t[i + 1][left_child])
			{
				t[i][j] = t[i + 1][right_child];
			}
		}
		child_num = father_num;
	}
}

template<typename T>
inline size_t losertree<T>::calculate_depth(size_t n)
{
	size_t depth = 1;
	size_t tmp = 1;
	while (tmp < n)
	{
		tmp *= 2;
		depth++;
	}
	return depth;
}

template<typename T>
inline bool losertree<T>::get_min(T* res)
{
	if ((*isvalid)(&t[0][0]) == false) {
		return false;
	}
	memmove(res, &t[0][0], sizeof(T));
	return true;
}

template<typename T>
inline void losertree<T>::set_value(size_t pos, T* value)
{
	memmove(&t[depth - 1][pos], value, sizeof(T));
	adjust(pos);
}

template<typename T>
inline void losertree<T>::adjust(size_t pos)
{
	size_t child_num = n;
	int son_pos = pos;
	for (int i = depth - 2; i >= 0; i--)
	{
		size_t father_pos = son_pos / 2;
		size_t left_child = 2 * father_pos;
		size_t right_child = 2 * father_pos + 1;
		t[i][father_pos] = t[i + 1][left_child];
		size_t small_pos = left_child;
		if (right_child < child_num && t[i + 1][right_child] < t[i + 1][left_child]) {
			t[i][father_pos] = t[i + 1][right_child];
			small_pos = right_child;
		}

		child_num = child_num / 2 + child_num % 2;
		son_pos = father_pos;
	}
}

template<typename T>
inline losertree<T>::~losertree()
{
	for (size_t i = 0; i < depth; i++)
	{
		delete t[i];
	}
	delete t;
}

