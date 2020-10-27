#include <cstdio>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include "UnitTest.h"
#include "LinearFunction.h"
#include "SortDecision.h"
using namespace std;

int testcmd(char* path_name, size_t count)
{
	ostringstream s;
	s << "dd if=/dev/zero of=";
	s << path_name;
	s << " bs=1M count=";
	s << count;
	std::cout << s.str() << std::endl;
	return system(s.str().c_str());
}

void testlosertree() {
	UnitTest test;
	test.TestLoserTree();
}

void testsort()
{
	UnitTest test;
	test.TestMergeSort();
}


int main(int argc, char* argv[])
{
	//a example to run the code, notice that: it is a cross development environment with windows visual studio development and linux running platform
	char* decision_file_path = "archieve.xml";
	char* test_data_file = "/home/hadoop/sdb/test/data.txt";
	char* tmp_file_path = "/home/hadoop/sdb/test/tmp.txt";
	char* sorted_file_path = "/home/hadoop/sdb/test/res.txt";
	size_t M_max = 1024 * 1024 * 1024;
	size_t M = 4 * 1024 * 1024;
	size_t B = 4096;
	size_t N = 8 * 1024 * 1024 * 1024ULL;
	SortDecision sortdecision(decision_file_path, test_data_file);
	tuple<double, size_t, size_t, size_t> res = sortdecision.Decide(N, M, M_max, B);
	//get the optimal scheme
	size_t k = get<1>(res);
	size_t Br = get<2>(res);
	size_t Bw = get<2>(res);

	//use the optimal scheme to run external mergesort
	Timer time;
	time.Start();
	template_mergesort<kv1> sort(k, M, Br, Bw);
	sort.sort(test_data_file, tmp_file_path, sorted_file_path, compare);
	time.Stop();
	time.ShowRunningTime();
	bool ascending = sort.check(sorted_file_path);
	if (ascending == true)
	{
		printf("ascending sorting successfully!\n");
	}
	else {
		printf("ascending sorting failed!\n");
	}

	printf("hello from MergeProject!\n");
	return 0;
}