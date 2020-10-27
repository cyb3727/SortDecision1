#include "Process.h"

Process::Process(double unit)
{
	this->previous = 0;
	this->unit = unit;
	Print(this->previous);
}

void Process::Print(double value)
{
	printf("Running:\t%2.1lf\%\n", value * 100);
}

void Process::PrintPercent(double now, double all)
{
#define eps 1.0e-6
	if (now < 0 || all < 0 || now > all + eps)
	{
		printf("%lf\t %lf\n", now, all);
		printf("Invalid State!\n");
		return;
	}

	double rate = now / all;
	if (rate + eps > previous + unit)
	{
		this->previous = this->previous + unit;
		Print(this->previous);
	}
}

Process::~Process()
{
}
