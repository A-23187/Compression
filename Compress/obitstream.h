#pragma once
#ifndef OBITSTREAM
#define OBITSTREAM
#include <fstream>
class obitstream {
	unsigned char buffer;
	char idx;
	std::ofstream fout;
public:
	constexpr static char end = -1;		//����ˢ��buffer������8λ��0
	constexpr static char end0 = -1;	//����ˢ��buffer������8λ��0
	constexpr static char end1 = -2;	//����ˢ��buffer������8λ��1
	obitstream(const char* filename):
		buffer(0), idx(7),
		fout(filename, std::ios::binary){}
	size_t size() const;
	void close();
	obitstream& operator<<(bool);
	obitstream& operator<<(int);
	obitstream& operator<<(char);
	obitstream& operator<<(unsigned char);
	bool writable() const;
	obitstream& write(const void*, std::streamsize);
	operator bool() const;
};
//	����buffer���ò��ֵĴ�С
inline size_t obitstream::size() const
{
	return idx + 1;
}
//	�ر�obitstream
inline void obitstream::close() 
{
	//�ڹر�ǰ����buffer��������bitλ����д��buffer
	if(idx < 7)	
		fout.write((char*)&buffer, sizeof buffer);
	fout.close();
}
//	д�뵥��bit
inline obitstream& obitstream::operator<<(bool bit)
{
	buffer |= bit << idx;
	idx--;
	if (idx == -1) {
		fout.write((char*)&buffer, sizeof buffer);
		buffer = 0;
		idx = 7;
	}
	return *this;
}
//	д�뵥��bit
inline obitstream& obitstream::operator<<(int bit) {
	if (bit == 0 || bit == 1) {
		buffer |= bit << idx;
		idx--;
		if (idx == -1) {
			fout.write((char*)&buffer, sizeof buffer);
			buffer = 0;
			idx = 7;
		}
	}
	return *this;
}
//	operator(end)������ǰbufferд�뵽�ļ��ڲ�ˢ��buffer
inline obitstream& obitstream::operator<<(char c)
{
	if ((c == end || c == end1) && idx < 7) {
		if (c == end1) {
			idx++;
			buffer += 1 << idx;
			buffer--;
		}
		fout.write((char*)&buffer, sizeof buffer);
		buffer = 0;
		idx = 7;
	}
	return *this;
}
//	д������� unsigned char
inline obitstream& obitstream::operator<<(unsigned char c)
{
	unsigned char tmp = c << (idx + 1);
	c = c >> (7 - idx) | buffer;
	buffer = tmp;
	fout.write((char *)&c, sizeof c);
	return *this;
}	 
//	�ж��Ƿ����˵��ú���write()
inline bool obitstream::writable() const
{
	return idx == 7;
}
//	write()��ֱ��������ǰbuffer�ﻺ���bit����srcָ�������д���ļ���
inline obitstream& obitstream::write(const void* src, std::streamsize count)
{
	fout.write((char *)src, count);
	return *this;
}
//	�ж�io״̬
inline obitstream::operator bool() const
{
	return bool(fout);
}
#endif