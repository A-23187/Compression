#pragma once
#ifndef SHANNONTREE
#define SHANNONTREE
#include <vector>
#include <stack>
#include <algorithm>
#include "CodeTree.h"
//	����ȡ��(log2(x))����x��2Ϊ�׵Ķ�������ȡ��
inline uint8_t upper_int_log2(unsigned x)
{
	x--;
	if (x == 0)
		return 0;
	else {
		int n = 31;
		if (!(x >> 16)) {
			n -= 16;
			x <<= 16;
		}
		if (!(x >> 24)) {
			n -= 8;
			x <<= 8;
		}
		if (!(x >> 28)) {
			n -= 4;
			x <<= 4;
		}
		if (!(x >> 30)) {
			n -= 2;
			x <<= 2;
		}
		n += (x >> 31);
		return n;
	}
}
//	��תuint64_t��bitλ
inline uint64_t reverse(uint64_t n)
{
	n = (n >> 32) | (n << 32);
	n = ((n & 0xffff0000ffff0000) >> 16) | ((n & 0x0000ffff0000ffff) << 16);
	n = ((n & 0xff00ff00ff00ff00) >> 8) | ((n & 0x00ff00ff00ff00ff) << 8);
	n = ((n & 0xf0f0f0f0f0f0f0f0) >> 4) | ((n & 0x0f0f0f0f0f0f0f0f) << 4);
	n = ((n & 0xcccccccccccccccc) >> 2) | ((n & 0x3333333333333333) << 2);
	n = ((n & 0xaaaaaaaaaaaaaaaa) >> 1) | ((n & 0x5555555555555555) << 1);
	return n;
}

//ShannonNode 
class ShannonNode {
	friend class ShannonTree;
	unsigned char key;
	uint8_t len;
public:
	ShannonNode() = default;
	ShannonNode(unsigned char key, uint8_t len) :key(key), len(len) {}
	bool operator<(const ShannonNode& that) {
		return this->len < that.len;
	}
};


//ShannonTree 
class ShannonTree :public CodeTree {
	std::vector<ShannonNode> nodes;
	void create();
public:
	ShannonTree() = default;
	ShannonTree(unsigned[], size_t);
	operator CodeTable() const override;
};

//	��ͳ�Ƶõ�����Ϣ�����ַ�����ÿ���ַ����ִ�����������ShannonTree
inline ShannonTree::ShannonTree(unsigned c[], size_t count)
{
	//��ÿ����Ҫ������ַ�����Ҫ���볤
	nodes.reserve(SIZE);
	unsigned tmp;
	for (size_t i = 0; i < SIZE; i++) {
		if (c[i]) {
			tmp = (unsigned)ceil(1.0 * count / c[i]);
			nodes.emplace_back(unsigned char(i), upper_int_log2(tmp));
		}
	}

	//���볤�ǵݼ�˳������
	sort(nodes.begin(), nodes.end());

	//���ݸ�����ShannonNode���鴴������
	create();
}
//	called in ctor
inline void ShannonTree::create()
{
	root = new CodeNode;
	CodeNode* p = root;
	unsigned i = 0, j = 0;
	std::stack<CodeNode*> s;
	while (true) {
		while (j < nodes[i].len) {
			s.push(p);
			p->left = new CodeNode;
			p = p->left;
			j++;
		}
		p->key = nodes[i++].key;
		if (i == nodes.size())
			break;
		p = s.top();
		while (p->right) {
			s.pop();
			if (s.empty())
				return;
			p = s.top();
		}
		p->right = new CodeNode;
		p = p->right;
		j = s.size();
	}
}
//	������ת��Ϊ��Ӧ�����
inline ShannonTree::operator CodeTable() const 
{
	if (!root)
		return false;
	//TODO ������˵��ֻ���ڴ�ѹ���ļ��ܴ������¸ú����Ż������

	CodeTable table;
	uint64_t u = 0;
	uint8_t len = nodes[0].len;
	for (const ShannonNode& node : nodes) {
		u <<= (node.len - len);
		table[node.key] = bitvcr(u++, len = node.len);
	}
	return table;
}
#endif
