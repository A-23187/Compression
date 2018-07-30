#pragma once
#ifndef HUFFMANTREE
#define HUFFMANTREE
#include <vector>
#include <stack>
#include <algorithm>
#include "CodeTree.h"
#include "bitstream.h"
//	������ HfmNode
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
	unsigned char* key;	//���ֵ����
	uint16_t* father;		//���ĸ��׽���������е��±������
	bool* _01;			//��ǰ����Ƿ�Ϊ�丸�׵��Һ���
	uint16_t count_of_leaf;			//Ҷ�ӽ����Ŀ
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

//	��ͳ�Ƶõ�����Ϣ��ÿ���ַ����ִ�����������HuffmanTree
inline HuffmanTree::HuffmanTree(unsigned c[]):count_of_leaf(0)
{
	//Ԥ����������count_of_leaf
	std::vector<HfmNode> nodes;
	nodes.reserve(SIZE);
	for (size_t i = 0; i < SIZE; i++) {
		if (c[i]) {
			nodes.emplace_back(c[i], count_of_leaf++, unsigned char(i));
		}
	}

	//new key��father��_01
	uint16_t i = 2 * count_of_leaf - 1;
	key = new unsigned char[i];
	father = new uint16_t[i];		//�����ĸ��׽����±�Ϊ i
	_01 = new bool[i - 1];

	//new left��right
	uint16_t* left = new uint16_t[i];
	uint16_t* right = new uint16_t[i];

	//initial
	father[i - 1] = i;
	for (uint16_t j = 0; j < count_of_leaf; j++) {
		left[j] = right[j] = i;
	}

	//����key��father����
	make_heap(nodes.begin(), nodes.end());
	size_t tmp;
	const HfmNode* node = nullptr;
	i = (uint16_t)nodes.size();
	while (nodes.size() > 1) {
		//ȡcount��С�Ľ�㣬������
		pop_heap(nodes.begin(), nodes.end());
		node = &nodes.back();
		tmp = node->count;
		key[node->idx] = node->key;
		father[node->idx] = i;
		_01[node->idx] = false;
		left[i] = node->idx;
		nodes.pop_back();

		//ȡcount����С�Ľ�㣬������
		pop_heap(nodes.begin(), nodes.end());
		node = &nodes.back();
		tmp += node->count;
		key[node->idx] = node->key;
		father[node->idx] = i;
		_01[node->idx] = true;
		right[i] = node->idx;
		nodes.pop_back();

		//���ϲ�����ѹ��
		nodes.emplace_back(tmp,i++);
		push_heap(nodes.begin(), nodes.end());
	}

	//������
	create(nodes[0], left, right);
	
	//�ͷ��ڴ�
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
//	������ת��Ϊ��Ӧ�����
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
//	���浽�ļ�
inline void HuffmanTree::save(obitstream& bout) const	//1��ʾҶ�ӽ�㣬0��ʾ�ڵ�
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
			//�������һ��������ж���ջ����Ƿ�Ϊnull����Ϊhfm�������ʣ��ڵ�����Һ��ӽ�ȫ
			s.push(p->right);
			s.push(p->left);
		}
	}
}
//	�������ļ�
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
		bin >> bit;	//������bit��ʾ�ղ�popǰ��top���(p���)�Ƿ�ΪҶ�ӽ��
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