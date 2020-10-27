#include "data.h"

bool data::checkallignment(char* buf)
{
	return false;
}

data::data(char* file_path, int mode)
{
	fd = open(file_path, mode, 0666);
	if (fd < 0)
	{
		printf("file open failed!\n");
	}
	strcpy(this->file_path, file_path);
	this->cur_pos = 0;
	this->data_size = getfilesize(file_path);
}

size_t data::getsize()
{
	return this->data_size;
}

void data::getfilepath(char* file_path)
{
	strcpy(file_path, this->file_path);
}

size_t data::readblock(char* buf, size_t blockId)
{
	return size_t();
}

run* data::createrun(size_t block_s, size_t block_e)
{
	run* r = new run(this, block_s, block_e);
	return r;
}

size_t data::readbuf(char* buf, size_t off, size_t len)
{
	lseek(fd, off, SEEK_SET);
	size_t read_size = read(fd, buf, len);
	return read_size;
}

size_t data::writebuf(char* buf, size_t off, size_t len)
{
	lseek(fd, off, SEEK_SET);
	size_t write_size = write(fd, buf, len);
	return write_size;
}

size_t data::append(char* buf, size_t size)
{
	lseek(fd, 0, SEEK_END);
	size_t append_size = write(fd, buf, size);
	/*for (int i = 0; i < 10; i++)
	{
		printf("%lld\n", *((long*)(buf + 16 * i)));
	}
	printf("flush data: %d\n", append_size);*/
	return append_size;
}

void data::clear()
{
	ftruncate(fd, 0);
}

size_t data::getfilesize(const char* file_name)
{
	struct stat statbuf;
	stat(file_name, &statbuf);
	size_t size = statbuf.st_size;
	return size;
}
