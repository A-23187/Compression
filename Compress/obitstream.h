#pragma once
#ifndef OBITSTREAM
#define OBITSTREAM
#include <fstream>
class obitstream {
	unsigned char buffer;
	char idx;
	std::ofstream fout;
public:
	constexpr static char end = -1;		//用于刷新buffer，不足8位补0
	constexpr static char end0 = -1;	//用于刷新buffer，不足8位补0
	constexpr static char end1 = -2;	//用于刷新buffer，不足8位补1
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
//	返回buffer可用部分的大小
inline size_t obitstream::size() const
{
	return idx + 1;
}
//	关闭obitstream
inline void obitstream::close() 
{
	//在关闭前，若buffer内有已用bit位，则写入buffer
	if(idx < 7)	
		fout.write((char*)&buffer, sizeof buffer);
	fout.close();
}
//	写入单个bit
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
//	写入单个bit
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
//	operator(end)，将当前buffer写入到文件内并刷新buffer
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
//	写入给定的 unsigned char
inline obitstream& obitstream::operator<<(unsigned char c)
{
	unsigned char tmp = c << (idx + 1);
	c = c >> (7 - idx) | buffer;
	buffer = tmp;
	fout.write((char *)&c, sizeof c);
	return *this;
}	 
//	判断是否适宜调用函数write()
inline bool obitstream::writable() const
{
	return idx == 7;
}
//	write()，直接跳过当前buffer里缓存的bit，将src指向的数据写入文件。
inline obitstream& obitstream::write(const void* src, std::streamsize count)
{
	fout.write((char *)src, count);
	return *this;
}
//	判断io状态
inline obitstream::operator bool() const
{
	return bool(fout);
}
#endif