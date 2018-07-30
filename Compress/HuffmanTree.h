#pragma once
#ifndef HUFFMANTREE
#define HUFFMANTREE
#include <vector>
#include <stack>
#include <algorithm>
#include "CodeTree.h"
#include "bitstream.h"
//	辅助类 HfmNode
class HfmNode {
	unsigned char key;
	size_t count;
	uint16_t idx;
public:
	HfmNode() = default;
	HfmNode(size_t count, uint16_t idx, unsigned char key = 0)
		:count(count), idx(idx), key(key) {}
	bool operator<(const HfmNode& that) {
		return this->count > that.count;
	}
	friend class HuffmanTree;
};
//	HuffmanTree
class HuffmanTree :public CodeTree {
	unsigned char* key;	//结点值数组
	uint16_t* father;		//结点的父亲结点在数组中的下标的数组
	bool* _01;			//当前结点是否为其父亲的右孩子
	uint16_t count_of_leaf;			//叶子结点数目
	void create(const HfmNode&, uint16_t*, uint16_t*);
public:
	HuffmanTree()
		:key(nullptr), father(nullptr), _01(nullptr), count_of_leaf(0) {}
	HuffmanTree(unsigned[]);
	operator CodeTable() const override;
	~HuffmanTree();
private:
	void save(obitstream&) const override;
	void load(ibitstream&) override;
};

//	由统计得到的信息（每个字符出现次数）来构造HuffmanTree
inline HuffmanTree::HuffmanTree(unsigned c[]):count_of_leaf(0)
{
	//预处理，且设置count_of_leaf
	std::vector<HfmNode> nodes;
	nodes.reserve(SIZE);
	for (size_t i = 0; i < SIZE; i++) {
		if (c[i]) {
			nodes.emplace_back(c[i], count_of_leaf++, unsigned char(i));
		}
	}

	//new key、father、_01
	uint16_t i = 2 * count_of_leaf - 1;
	key = new unsigned char[i];
	father = new uint16_t[i];		//根结点的父亲结点的下标为 i
	_01 = new bool[i - 1];

	//new left、right
	uint16_t* left = new uint16_t[i];
	uint16_t* right = new uint16_t[i];

	//initial
	father[i - 1] = i;
	for (uint16_t j = 0; j < count_of_leaf; j++) {
		left[j] = right[j] = i;
	}

	//设置key、father内容
	make_heap(nodes.begin(), nodes.end());
	size_t tmp;
	const HfmNode* node = nullptr;
	i = (uint16_t)nodes.size();
	while (nodes.size() > 1) {
		//取count最小的结点，并弹出
		pop_heap(nodes.begin(), nodes.end());
		node = &nodes.back();
		tmp = node->count;
		key[node->idx] = node->key;
		father[node->idx] = i;
		_01[node->idx] = false;
		left[i] = node->idx;
		nodes.pop_back();

		//取count次最小的结点，并弹出
		pop_heap(nodes.begin(), nodes.end());
		node = &nodes.back();
		tmp += node->count;
		key[node->idx] = node->key;
		father[node->idx] = i;
		_01[node->idx] = true;
		right[i] = node->idx;
		nodes.pop_back();

		//结点合并，并压入
		nodes.emplace_back(tmp,i++);
		push_heap(nodes.begin(), nodes.end());
	}

	//创建树
	create(nodes[0], left, right);
	
	//释放内存
	delete[] left;
	delete[] right;
}
//	called in ctor
inline void HuffmanTree::create(const HfmNode& root, uint16_t* left, uint16_t* right)
{
	std::stack<uint16_t> s;
	s.push(root.idx);

	std::stack<CodeNode*> s1;
	this->root = new CodeNode;
	s1.push(this->root);

	size_t t;
	CodeNode* p = nullptr;
	const uint16_t null = 2 * count_of_leaf - 1;
	while (!s.empty()) {
		t = s.top();
		p = s1.top();
		s.pop();
		s1.pop();
		if (right[t] != null) {
			s.push(right[t]);
			p->right = new CodeNode(key[right[t]]);
			s1.push(p->right);
		}
		if (left[t] != null) {
			s.push(left[t]);
			p->left = new CodeNode(key[left[t]]);
			s1.push(p->left);
		}
	}
}
//	将码树转换为对应的码表
inline HuffmanTree::operator CodeTable() const
{
	if (!root)
		return false;
	CodeTable table;
	const uint16_t null = 2 * count_of_leaf - 1;
	size_t size;
	uint16_t j;
	uint64_t u;
	for (uint16_t i = 0; i < count_of_leaf; i++) {
		size = 0;
		u = 0;
		j = i;
		while (father[j] != null) {
			u += (_01[j] << size);
			j = father[j];
			size++;
		}
		table[key[i]] = bitvcr(u,size);
	}
	return table;
}//	dtor
inline HuffmanTree::~HuffmanTree()
{
	delete[] key;
	delete[] father;
	delete[] _01;
}
//	保存到文件
inline void HuffmanTree::save(obitstream& bout) const	//1表示叶子结点，0表示内点
{
	std::stack<CodeNode*> s;
	s.push(root);
	CodeNode* p = nullptr;
	while (!s.empty()) {
		p = s.top();
		s.pop();
		if (p->isLeaf())
			bout << 1 << p->key;
		else {
			bout << 0;
			//这里和下一句均不用判断入栈结点是否为null，因为hfm树的性质：内点皆左右孩子健全
			s.push(p->right);
			s.push(p->left);
		}
	}
}
//	加载自文件
inline void HuffmanTree::load(ibitstream& bin)
{
	bool bit;
	root = new CodeNode;
	std::stack<CodeNode*> s;
	s.push(root);
	CodeNode* p;
	while (!s.empty()) {
		p = s.top();
		s.pop();
		bin >> bit;	//读到的bit表示刚才pop前的top结点(p结点)是否为叶子结点
		if (bit) {
			bin >> p->key;
		}
		else {
			p->right = new CodeNode;
			p->left = new CodeNode;
			s.push(p->right);
			s.push(p->left);
		}
	}
}
#endif