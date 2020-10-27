#include "mergesort.h"

mergesort::mergesort(size_t k, size_t M, size_t Br, size_t Bw)
{
	if (k * Br + Bw > M)
	{
		printf("Mergesort setting has error!\n");
		return;
	}

	this->mem = new char[M];
	for (size_t i = 0; i < k; i++)
	{
		Brs.push_back(this->mem + i * Br);
	}
	this->Bw = this->mem + k * Br;
}

bool mergesort::merge_run(char* data_file, size_t M, int rw_unit, int kv_len, size_t k, char* file_res)
{
	data* sort_data = new data(data_file, O_RDWR | O_DIRECT);
	this->sort_data = sort_data;
}

void mergesort::format_run(char* data_file, size_t M, int rw_unit, int kv_unit, std::vector<run*>* runs, int (*cmp)(const void*, const void*))
{
	if (M % rw_unit != 0)
	{
		printf("Memory M:%lld should be alligned with %lldB\n", M, rw_unit);
		return;
	}

	size_t data_size = sort_data->getsize();
	size_t block_num = data_size / rw_unit;
	size_t ele_num = data_size / kv_unit;
	size_t M_num = M / rw_unit;
	size_t run_num = block_num / M_num;
	if (block_num % M_num > 0)
	{
		run_num += 1;
	}

	for (size_t i = 0; i < run_num; i++)
	{
		run* r = NULL;
		if (M_num * (i + 1) < block_num) {
			r = sort_data->createrun(M_num * i, M_num * (i + 1) - 1);
		}
		else {
			r = sort_data->createrun(M_num * i, block_num - 1);
		}
		runs->push_back(r);

		//read data into mem
		size_t off = r->get_s() * rw_unit;
		size_t size = (r->get_e() - r->get_s() + 1) * rw_unit;
		sort_data->readbuf(mem, off, size);

		//sort mem in ascending order
		qsort(mem, size / kv_unit, kv_unit, *cmp);

		//write data back to mem
		sort_data->writebuf(mem, off, size);
	}
}

void mergesort::merge_k_run(std::vector<run*>* runs, run* r, data* target_data)
{
	//merge runs into one run
	size_t block_s, block_e;
	size_t size = runs->size();
	for (size_t i = 0; i < size; i++)
	{
		if (i == 0)
		{
			block_s = (*runs)[i]->get_s();
			block_e = (*runs)[i]->get_e();
		}
		else
		{
			block_s = std::min(block_s, (*runs)[i]->get_s());
			block_e = std::max(block_e, (*runs)[i]->get_e());
		}
	}

	char* buf = this->Bw;
	size_t off = 0;
	size_t len = 0;
	//build a loser tree
	//when the buf is full then flush it to target data file
	/*while (getmin() == true) {
		if (对应的run的block没用完)
		{
			if (缓存用完) {
				加载数据进缓存
			}
			添加进一个新元素
		}
		else {
			添加一个无效数值
		}
		把最小值写到缓存中
		if (缓存满) {
			写到外存中去
		}
	}*/

	//load data from k runs and merge them in ascending order into new run r
	// when the buffer of Bw is full and then write it into target_data
	target_data->writebuf(buf, off, len);

	r = target_data->createrun(block_s, block_e);
}

mergesort::~mergesort()
{
	delete mem;
}
