#pragma once
#ifndef CODETABLE
#define CODETABLE
#include <stdint.h>
#include <iostream>
#include "bitstream.h"
constexpr size_t SIZE = 256;	//字符集大小


//	bit向量，大小固定，最大只能有56位
union bitvcr {
public:
	unsigned char _size;
	uint64_t _ref;	//[_ref](8byte) = [data](7byte) + [_size](1byte) !!!
public:
	bitvcr() :_ref(0){}
	bitvcr(uint64_t, size_t);
	size_t size() const;
	bool empty() const;
	bool operator[](size_t) const;
	void push_back(bool);
	friend obitstream& operator<<(obitstream&, const bitvcr&);
	friend std::ostream& operator<<(std::ostream&, const bitvcr&);
};
//	ctor
inline bitvcr::bitvcr(uint64_t data, size_t size)
{
	//	不作上溢出检测
	_ref = data << 8;
	_ref += size;
}
//	返回大小
inline size_t bitvcr::size() const
{
	return _size;
}
//	是否为空vcr
inline bool bitvcr::empty() const
{
	return _size == 0;
}
//	取pos处的bit位，不作越界检测
inline bool bitvcr::operator[](size_t pos) const
{
	return _ref >> (7 + _size - pos) & 1;
}
//	往后面追加bit
inline void bitvcr::push_back(bool bit)
{
	unsigned char tmp = _size + 1;
	_ref = ((_ref >> 8 << 1) + bit) << 8;
	_ref += tmp;
}
//	cout << bitvcr
inline std::ostream& operator<<(std::ostream& cout, const bitvcr& bitv)
{
	for (size_t pos = 0; pos < bitv._size; pos++) {
		cout << bitv[pos];
	}
	return cout;
}
//	通过obitstream将bitvcr写入文件
inline obitstream& operator<<(obitstream& bout, const bitvcr& bitv)
{
	size_t r = bitv._size % 8;	// r为余数
	size_t n = bitv._size - r;	// n为8的整数倍，满足 _size = n + r
	uint64_t tmp = bitv._ref << (56 - bitv._size);
	for (size_t i = 0; i < n; i += 8) {
		bout << unsigned char(tmp >> (56 - i));
	}
	for (size_t i = 0; i < r; i++) {
		bout << bitv[n + i];
	}
	return bout;
}


//	码表，用于将字符(unsigned char)映射为比特向量(bitvcr)
class CodeTable {
	bool tag;	//用于标记该表是否可用
	bitvcr table[SIZE];
public:
	CodeTable(bool tag = true) :tag(tag) {}
	bitvcr& operator[](size_t);
	operator bool() const;
	bool operator~();
};
//	取pos处的bitvcr，不作越界检测
bitvcr& CodeTable::operator[](size_t pos)
{
	return table[pos];
}
//	返回表的可用状态
CodeTable::operator bool() const
{
	return tag;
}
//	修改表的可用状态
bool CodeTable::operator~()
{
	return tag = !tag;
}
#endif // CODETABLE
