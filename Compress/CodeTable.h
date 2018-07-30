#pragma once
#ifndef CODETABLE
#define CODETABLE
#include <stdint.h>
#include <iostream>
#include "bitstream.h"
constexpr size_t SIZE = 256;	//�ַ�����С


//	bit��������С�̶������ֻ����56λ
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
	//	������������
	_ref = data << 8;
	_ref += size;
}
//	���ش�С
inline size_t bitvcr::size() const
{
	return _size;
}
//	�Ƿ�Ϊ��vcr
inline bool bitvcr::empty() const
{
	return _size == 0;
}
//	ȡpos����bitλ������Խ����
inline bool bitvcr::operator[](size_t pos) const
{
	return _ref >> (7 + _size - pos) & 1;
}
//	������׷��bit
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
//	ͨ��obitstream��bitvcrд���ļ�
inline obitstream& operator<<(obitstream& bout, const bitvcr& bitv)
{
	size_t r = bitv._size % 8;	// rΪ����
	size_t n = bitv._size - r;	// nΪ8�������������� _size = n + r
	uint64_t tmp = bitv._ref << (56 - bitv._size);
	for (size_t i = 0; i < n; i += 8) {
		bout << unsigned char(tmp >> (56 - i));
	}
	for (size_t i = 0; i < r; i++) {
		bout << bitv[n + i];
	}
	return bout;
}


//	������ڽ��ַ�(unsigned char)ӳ��Ϊ��������(bitvcr)
class CodeTable {
	bool tag;	//���ڱ�Ǹñ��Ƿ����
	bitvcr table[SIZE];
public:
	CodeTable(bool tag = true) :tag(tag) {}
	bitvcr& operator[](size_t);
	operator bool() const;
	bool operator~();
};
//	ȡpos����bitvcr������Խ����
bitvcr& CodeTable::operator[](size_t pos)
{
	return table[pos];
}
//	���ر�Ŀ���״̬
CodeTable::operator bool() const
{
	return tag;
}
//	�޸ı�Ŀ���״̬
bool CodeTable::operator~()
{
	return tag = !tag;
}
#endif // CODETABLE
