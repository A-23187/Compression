#pragma once
#ifndef IBITSTREAM
#define IBITSTREAM
#include <fstream>
class ibitstream {
	unsigned char buffer;
	char idx;
	std::ifstream fin;
public:
	ibitstream(const char* filename) :idx(-1),
		fin(filename, std::ios::binary) {}
	size_t size() const;
	void close();
	ibitstream& operator>>(bool&);
	ibitstream& operator>>(unsigned char&);
	ibitstream& read(void*, std::streamsize, unsigned char&);
	operator bool() const;
};

//	返回buffer可用部分的大小
inline size_t ibitstream::size() const
{
	return 7 - idx;
}
//	关闭ibitstream
inline void ibitstream::close()
{
	fin.close();
}
//	从文件读入单个bit
inline ibitstream& ibitstream::operator>>(bool& bit)
{
	if (idx == -1) {
		fin.read((char *)&buffer, sizeof buffer);
		if (!fin)
			return *this;
		else idx = 7;
	}
	bit = buffer >> idx & 1;
	idx--;
	return *this;
}
//	从文件读入一个unsigned char
inline ibitstream& ibitstream::operator>>(unsigned char& c)
{
	if (idx == -1) {
		fin.read((char *)&buffer, sizeof buffer);
		if (!fin)
			return *this;
		else idx = 7;
	}
	if (idx == 7) {
		c = buffer;
		idx = -1;
	}	
	else {
		c = buffer << (7 - idx);
		if (fin.read((char *)&buffer, sizeof buffer)) {
			c |= buffer >> (idx + 1);
		}
	}
	return *this;
}
//	read()，功能和ifstream的read函数一样，且同时通过第3个参数返回原先的buffer
inline ibitstream& ibitstream::read(void* des, std::streamsize count, unsigned char& buffer)
{
	fin.read((char *)des, count);
	buffer = this->buffer;
	idx = -1;
	return *this;
}
//	判断io状态
inline ibitstream::operator bool() const
{
	return bool(fin);
}

#endif
