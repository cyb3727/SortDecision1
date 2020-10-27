#pragma once
#include <cstdio>

class Process
{
	double unit;
	double previous;
	void Print(double value);
public:

	/**
	 * print the process of a running application
	 * parameter unit: the unit of printed information
	 */
	Process(double unit);

	/**
	 * print the process
	 * parameter now: the now status
	 * parameter all: the total number
	 */
	void PrintPercent(double now, double all);
	~Process();
};

