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

//	����buffer���ò��ֵĴ�С
inline size_t ibitstream::size() const
{
	return 7 - idx;
}
//	�ر�ibitstream
inline void ibitstream::close()
{
	fin.close();
}
//	���ļ����뵥��bit
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
//	���ļ�����һ��unsigned char
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
//	read()�����ܺ�ifstream��read����һ������ͬʱͨ����3����������ԭ�ȵ�buffer
inline ibitstream& ibitstream::read(void* des, std::streamsize count, unsigned char& buffer)
{
	fin.read((char *)des, count);
	buffer = this->buffer;
	idx = -1;
	return *this;
}
//	�ж�io״̬
inline ibitstream::operator bool() const
{
	return bool(fin);
}

#endif
