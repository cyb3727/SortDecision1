#include "Randomer.h"

Randomer::Randomer()
{
	srand((unsigned)time(NULL));
	/* Seed */
	rd = new std::random_device();
	/* Random number generator */
	std::default_random_engine* generator_ptr = new std::default_random_engine((*rd)());
}

double Randomer::randunit()
{
	return (1.0 * rand()) / RAND_MAX;
}

int Randomer::random(int start, int end)
{
	if (start == end) return start;
	int mod = end - start;

	//It means start=0 and end=RAND_MAX
	if (mod == RAND_MAX)
	{
		return rand();
	}
	else
	{
		return start + (rand() % (mod + 1));
	}
}

ulong Randomer::ulong_random(ulong start, ulong end)
{
	/* Distribution on which to apply the generator */
	std::uniform_int_distribution<long long unsigned> distribution(start, end - 1);

	return distribution(*generator_ptr);
}

Randomer::~Randomer()
{
//	delete generator_ptr;
	delete rd;
}
