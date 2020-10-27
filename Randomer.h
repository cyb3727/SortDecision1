#pragma once
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <random>

class Randomer
{
	std::default_random_engine* generator_ptr;
	std::random_device* rd;
public:
	Randomer();
	/**
	 * generate a [0,1] uniform random number
	 * return: a double uniform random number in [0,1]
	 */
	double randunit();

	/**
	 * generate a [start,end] uniform random number
	 * return: a uniform interge random number in [start,end]
	 */
	int random(int start, int end);

	/**
	* generate a [start,end] uniform random number
	* return: a uniform interge random number in [start,end]
	*/
	ulong ulong_random(ulong start, ulong end);
	~Randomer();
};

