#include "UnitTest.h"

void UnitTest::GenerateLongSortData(char* file_path, ulong file_size, int block_size)
{
	FILE* data_file = fopen(file_path, "w");
	if (data_file == NULL)
	{
		printf("%s open failed!\n", data_file);
		return;
	}
	if (file_size % (2 * sizeof(ulong)) != 0)
	{
		printf("file size must be divided by %d!\n", 2 * sizeof(ulong));
		return;
	}
	ulong num = file_size / (2 * sizeof(ulong));
	ulong write_size = 1024 * 1024 * 100;//100M key-value pair
	ulong* write_buf = new ulong[write_size];
	ulong up_bound = write_size / 2;

	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<long long unsigned> distribution(0, 0xFFFFFFFFFFFFFFFF);

	ulong offset = 0;
	for (ulong i = 0; i < num; i++, offset++)
	{
		write_buf[2 * offset] = distribution(generator);
		write_buf[2 * offset + 1] = distribution(generator);

		if ((i + 1) % up_bound == 0 || i == num - 1)
		{
			//write data into file
			size_t w_size = fwrite(write_buf, 2 * sizeof(ulong), offset + 1, data_file);
			memset(write_buf, 0, write_size * sizeof(ulong));
			printf("write data %llu\n", 2 * sizeof(ulong) * w_size);
			offset = -1;
		}
	}
	delete[] write_buf;
	fclose(data_file);
}

int UnitTest::TestBlockManagerRandReadSpeed(int block_size, const char* file_name, double& bandwidth, int seconds)
{
	size_t file_size = GetFileSize(file_name);
	if (file_size % block_size != 0)
	{
		printf("The file_size: %u should be divided by %u\n", file_size, block_size);
		return FAIL;
	}

	ulong num = file_size / block_size;
	int file_mode = 0;
	BlockManager* man = new BlockManager(file_name, file_mode, block_size);
	Timer tm;
	Randomer random;
	Process proc(0.01);

	char* des = new char[block_size];
	memset(des, 0, block_size);
	printf("Rand Read Test Start!\n");
	tm.Start();
	size_t IO_size = 0;
	double seconds_past = -1;
	do {
		ulong blockId = random.random(0, num - 1);
		if (man->ReadBlock(des, blockId, block_size) != SUCCESS)
		{
			printf("UnitTest TestBlockManager: Rand Read Test Failed!\n");
			delete des;
			delete man;
			return FAIL;
		}
		IO_size += block_size;
		seconds_past = tm.GetRunningTime();
		proc.PrintPercent(seconds_past, seconds);
	} while (seconds_past < seconds);
	/*for (ulong i = 0; i < num; i++) {
		ulong blockId = random.random(0, num - 1);
		if (man->ReadBlock(des, blockId, block_size) != SUCCESS)
		{
			printf("UnitTest TestBlockManager: Rand Read Test Failed!\n");
			delete des;
			delete man;
			return FAIL;
		}
		proc.PrintPercent(i, num);
	}*/
	tm.Stop();
	tm.ShowRunnedTime();
	/*bandwidth = file_size / tm.GetTimeInterval() / (1024 * 1024);
	printf("the speed of rand read is: %.2lf MB/s\n", file_size / tm.GetTimeInterval() / (1024 * 1024));*/
	bandwidth = IO_size / tm.GetTimeInterval() / (1024 * 1024);
	printf("the speed of rand read is: %.2lf MB/s\n", IO_size / tm.GetTimeInterval() / (1024 * 1024));
	printf("Rand Read Test End!\n");
	delete des;
	delete man;
	return SUCCESS;
}

int UnitTest::TestBlockManagerRandWriteSpeed(int block_size, const char* file_name, double& bandwidth, int seconds)
{
	size_t file_size = GetFileSize(file_name);
	if (file_size % block_size != 0)
	{
		printf("The file_size: %u should be divided by %u\n", file_size, block_size);
		return FAIL;
	}

	ulong num = file_size / block_size;
	int file_mode = O_RDWR;
	BlockManager* man = new BlockManager(file_name, file_mode, block_size);
	Timer tm;
	Randomer random;
	Process proc(0.01);

	char* src = new char[block_size];
	memset(src, 0x1234, block_size);
	printf("Rand Write Test Start!\n");
	tm.Start();
	size_t IO_size = 0;
	double seconds_past = -1;
	do {
		ulong blockId = random.random(0, num - 1);
		if (man->WriteBlock(src, blockId, block_size) != SUCCESS)
		{
			printf("UnitTest TestBlockManager: Rand Write Test Failed!\n");
			delete src;
			delete man;
			return FAIL;
		}
		IO_size += block_size;
		seconds_past = tm.GetRunningTime();
		proc.PrintPercent(seconds_past, seconds);
	} while (seconds_past < seconds);
	/*for (ulong i = 0; i < num; i++) {
		ulong blockId = random.random(0, num - 1);
		if (man->WriteBlock(src, blockId, block_size) != SUCCESS)
		{
			printf("UnitTest TestBlockManager: Rand Write Test Failed!\n");
			delete src;
			delete man;
			return FAIL;
		}
		proc.PrintPercent(i, num);
	}*/
	tm.Stop();
	tm.ShowRunnedTime();
	/*bandwidth = file_size / tm.GetTimeInterval() / (1024 * 1024);
	printf("the speed of rand write is: %.2lf MB/s\n", file_size / tm.GetTimeInterval() / (1024 * 1024));*/
	bandwidth = IO_size / tm.GetTimeInterval() / (1024 * 1024);
	printf("the speed of rand read is: %.2lf MB/s\n", IO_size / tm.GetTimeInterval() / (1024 * 1024));

	printf("Rand Write Test End!\n");
	delete src;
	delete man;
	return SUCCESS;
}

void UnitTest::TestMergeSort()
{
	size_t k = 2;
	size_t M = 1024ULL * 1024 * 64;
	size_t Br = 1024ULL * 1024 * 30;
	size_t Bw = M - k * Br;
	template_mergesort<kv1> sort(k, M, Br, Bw);
	sort.sort("/home/hadoop/sdb/test/data.txt", "/home/hadoop/sdb/test/tmp.txt", "/home/hadoop/sdb/test/res.txt", compare);
	bool ascending = sort.check("/home/hadoop/sdb/test/res.txt");
	if (ascending == true)
	{
		printf("ascending sorting successfully!\n");
	}
	else {
		printf("ascending sorting failed!\n");
	}
}

void UnitTest::TestLoserTree()
{
	std::vector<node> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(node(v.size(), i, i, true));
	}
	losertree<node> tree(&v, isvalid);
	node res;
	tree.get_min(&res);
	std::cout << *((long*)res.kv) << " " << *((long*)(res.kv + 4)) << std::endl;
	node s(res.pos, 5, 4, true);
	tree.set_value(res.pos, &s);

	for (int i = 0; i < 11; i++) {
		if (tree.get_min(&res) == false) {
			break;
		}
		std::cout << *((long*)res.kv) << " " << *((long*)(res.kv + 4)) << std::endl;
		node s1(res.pos, -1, -1, false);
		tree.set_value(res.pos, &s1);
	}
}
