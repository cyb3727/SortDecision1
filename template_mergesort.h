#pragma once
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include "run.h"
#include "data.h"
#include "block.h"
#include "losenode.h"
#include "losertree.h"
#include "outbuffer.h"

template<typename kv>
class template_mergesort
{
	size_t k;
	size_t Br_size;
	size_t Bw_size;
	size_t M_size;
	std::vector<char*> Brs;
	char* Bw;
	char* mem;

	void run_release(std::vector<run*>* runs)
	{
		size_t size = runs->size();
		for (size_t i = 0; i < size; i++)
		{
			delete runs->at(i);
		}
		runs->clear();
	}

	int renamefile(char* file_path, char* new_name)
	{
		std::ostringstream s;
		s << "mv " << file_path << " " << new_name;
		std::cout << s.str() << std::endl;
		return system(s.str().c_str());
	}
public:
	template_mergesort(size_t k, size_t M, size_t Br, size_t Bw);

	/**
	 * data_file: the data to be sorted
	 * tmp_file: the temporary data
	 */
	void sort(char* data_file, char* tmp_file, char* file_res, int (*cmp)(const void*, const void*));

	/**
	 * check whether the data is really ascending sorted
	 */
	bool check(char* data_file)
	{
		std::ifstream inf;
		inf.open(data_file, std::ios::in | std::ios::binary);
		bool first_set = true;
		kv pre;
		kv now;
		size_t count = 0;
		while (inf.peek() != EOF)
		{
			if (first_set == true)
			{
				inf.read((char*)&pre, sizeof(kv));
				first_set = false;
			}
			else
			{
				inf.read((char*)&now, sizeof(kv));
				if (now < pre)
				{
					inf.close();
					return false;
				}
				memmove(&pre, &now, sizeof(kv));
			}
			count += sizeof(kv);
			/*if (count % 10000 == 0) {
				printf("%lld\n", count);
			}*/
		}
		inf.close();
		printf("check total count of %lld\n", count);
		return true;
	}

	/**
	 * parameter data_file: the data to be sorted
	 * parameter M: the memory
	 * parameter rw_unit: the read/write unit
	 * parameter kv_len: kv length
	 * parameter k: k-way merge sorting
	 * parameter tmp_file: the tmp_file used to store intermedia data
	 */
	bool run_merge(data* src_data, data* tmp_data, char* file_res, std::vector<run*>* runs);

	/**
	 * parameter data_file: the source data
	 * parameter rw_unit: the read/write unit
	 * parameter kv_unit: the kv unit
	 * parameter runs: the formation runs
	 * parameter cmp: the comparing function
	 */
	void run_format(data* sort_data, std::vector<run*>* runs, int (*cmp)(const void*, const void*));

	/**
	 * load data from runs and merge them in ascending order into new run r which belongs to target_data
	 * parameter runs: the runs to be merged
	 * parameter run: the merged run
	 * parameter target_data: the data to be saved
	 */
	void merge_runs(std::vector<run*>* runs, run*& r, data* target_data);

	/**
	 * merge runs with k-way merging into merged_runs
	 * parameter runs:runs to be merged
	 * parameter merged_runs: merged run result
	 * parameter k: k-way merging
	 * target_data: merged runs' data is stored into target_data
	 */
	void merge_runs_by_k(std::vector<run*>* runs, std::vector<run*>* merged_runs, size_t k, data* target_data);

	~template_mergesort();
};

template<typename kv>
inline template_mergesort<kv>::template_mergesort(size_t k, size_t M, size_t Br, size_t Bw)
{
	if (k * Br + Bw > M)
	{
		printf("Mergesort setting has error!\n");
		return;
	}

	this->M_size = M;
	this->Br_size = Br;
	this->Bw_size = Bw;
	this->k = k;

	//this->mem = new char[M];
	if (posix_memalign((void**)&(this->mem), 512, M) < 0) {
		printf("Allocate Memory Failed!\n");
	}

	for (size_t i = 0; i < k; i++)
	{
		Brs.push_back(this->mem + i * Br);
	}
	this->Bw = this->mem + k * Br;
}

template<typename kv>
inline void template_mergesort<kv>::sort(char* data_file, char* tmp_file, char* file_res, int (*cmp)(const void*, const void*))
{
	data* src_data = new data(data_file, O_DIRECT | O_RDWR);
	data* tmp_data = new data(tmp_file, O_CREAT | O_DIRECT | O_RDWR);
	std::vector<run*>* runs = new std::vector<run*>();

	run_format(src_data, runs, cmp);
	run_merge(src_data, tmp_data, file_res, runs);

	delete runs;
	delete tmp_data;
	delete src_data;
}

template<typename kv>
inline bool template_mergesort<kv>::run_merge(data* src_data, data* tmp_data, char* file_res, std::vector<run*>* runs)
{
	data** datas = new data * [2];
	datas[0] = src_data;
	datas[1] = tmp_data;

	std::vector<run*>** runs_p = new std::vector<run*> * [2];
	runs_p[0] = new std::vector<run*>();
	runs_p[1] = new std::vector<run*>();

	runs_p[0] = runs;
	int s_index = 0;
	int t_index = 1;


	while (runs_p[s_index]->size() > 1)
	{
		//clear the target file data to make sure that append function works rightly
		datas[t_index]->clear();
		merge_runs_by_k(runs_p[s_index], runs_p[t_index], k, datas[t_index]);
		run_release(runs_p[s_index]);

		s_index ^= t_index;
		t_index ^= s_index;
		s_index ^= t_index;
	}
	char file_path[256];
	datas[s_index]->getfilepath(file_path);
	printf("final sorted data file is: %s\n", file_path);
	renamefile(file_path, file_res);
	delete runs_p;
	delete datas;
}

template<typename kv>
inline void template_mergesort<kv>::run_format(data* sort_data, std::vector<run*>* runs, int(*cmp)(const void*, const void*))
{
	size_t kv_unit = sizeof(kv);
	size_t data_size = sort_data->getsize();
	size_t block_num = data_size / Br_size + (data_size % Br_size > 0);
	size_t ele_num = data_size / kv_unit;
	size_t M_num = M_size / Br_size;
	size_t run_num = block_num / M_num + (block_num % M_num > 0);

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
		size_t off = r->get_s() * Br_size;
		size_t size = (r->get_e() - r->get_s() + 1) * Br_size;

		//the last block may not be full
		if (i == run_num - 1)
		{
			size = data_size - off;
		}
		sort_data->readbuf(mem, off, size);

		//sort mem in ascending order
		qsort(mem, size / kv_unit, kv_unit, *cmp);

		//write data back to mem
		sort_data->writebuf(mem, off, size);
	}
}

template<typename kv>
inline void template_mergesort<kv>::merge_runs(std::vector<run*>* runs, run*& r, data* target_data)
{
	//merge runs into one run
	std::vector<block> bs;
	std::vector<losenode<kv> > v;
	outbuffer out_stream;
	out_stream.set(Bw, Bw_size, target_data);

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

	//load first block of each run into brs buffers
	for (size_t i = 0; i < size; i++)
	{
		block b;
		(*runs)[i]->readseqblock(&b, Brs[i], Br_size);
		bs.push_back(b);
	}

	for (size_t i = 0; i < size; i++)
	{
		kv key_value;
		bs[i].getkv(&key_value, sizeof(kv));
		losenode<kv> a(i, key_value, true);
		v.push_back(a);
	}
	losertree<losenode<kv> > tree(&v, isvalid);

	bool empty = false;
	size_t total_count = 0;
	kv pre;
	while (true)
	{
		losenode<kv> tmp_n;
		empty = !tree.get_min(&tmp_n);
		if (empty == true) break;
		size_t pos = tmp_n.pos;
		losenode<kv> new_node;
		if (bs[pos].end() == true && (*runs)[pos]->endofrun() == true)
		{
			new_node.setinvalid(pos);
			//std::cout << new_node.key_value << std::endl;
		}
		else if (bs[pos].end() == true && (*runs)[pos]->endofrun() == false)
		{
			(*runs)[pos]->readseqblock(&bs[pos], Brs[pos], Br_size);
			kv key_value;
			bs[pos].getkv(&key_value, sizeof(kv));
			new_node.set(pos, key_value, true);
			//std::cout << key_value << std::endl;
		}
		else if (bs[pos].end() == false)
		{
			kv key_value;
			bs[pos].getkv(&key_value, sizeof(kv));
			new_node.set(pos, key_value, true);
			//std::cout << key_value << std::endl;
		}
		tree.set_value(pos, &new_node);

		//write tmp_n into output buffer
		out_stream.append(&(tmp_n.key_value), sizeof(kv));
		if (total_count > 0)
		{
			if (tmp_n.key_value < pre)
			{
				printf("error");
			}
		}
		memmove(&pre, &(tmp_n.key_value), sizeof(kv));
		total_count += sizeof(kv);
	}

	printf("total count is: %lld\n", total_count);
	//close the stream and make sure that all the data flush to external memory
	out_stream.close();

	//create a new run of sorted data of runs 
	r = target_data->createrun(block_s, block_e);
}

template<typename kv>
inline void template_mergesort<kv>::merge_runs_by_k(std::vector<run*>* runs, std::vector<run*>* merged_runs, size_t k, data* target_data)
{
	size_t size = runs->size();
	std::vector<run*> tmp_runs;
	for (size_t i = 0; i < size; i++)
	{
		tmp_runs.push_back(runs->at(i));
		if (tmp_runs.size() == k)
		{
			run* create_run;
			merge_runs(&tmp_runs, create_run, target_data);
			merged_runs->push_back(create_run);

			tmp_runs.clear();
		}
	}

	if (tmp_runs.size() > 0)
	{
		run* create_run;
		merge_runs(&tmp_runs, create_run, target_data);
		merged_runs->push_back(create_run);

		tmp_runs.clear();
	}
}

template<typename kv>
inline template_mergesort<kv>::~template_mergesort()
{
	delete mem;
}
