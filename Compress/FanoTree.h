#pragma once
#ifndef FANOTREE
#define FANOTREE
#include <vector>
#include <algorithm>
#include "CodeTree.h"
//	������ FanoNode
class FanoNode {
	unsigned char key;
	size_t count;	//ǰn���
public:
	FanoNode() = default;
	FanoNode(unsigned char key, size_t count) :key(key),count(count){}
	bool operator<(const FanoNode& that) {
		return this->count < that.count;
	}
	friend class FanoTree;
};


class FanoTree :public CodeTree {
	std::vector<FanoNode> nodes;
	void create(CodeNode*&, uint16_t, uint16_t);
public:
	FanoTree() = default;
	FanoTree(unsigned[]);
private:
	void save(obitstream&) const override;
	void load(ibitstream&) override;
};

//	��ͳ�Ƶõ�����Ϣ��ÿ���ַ����ִ�����������FanoTree
inline FanoTree::FanoTree(unsigned c[])
{
	//����nodes������count��Ϊ�ַ����ִ���
	nodes.reserve(SIZE);
	nodes.emplace_back(0, 0);	//���ɣ����ǰ���ڱ�
	for (size_t i = 0; i < SIZE; i++) {
		if (c[i]) {
			nodes.emplace_back(unsigned char(i), c[i]);
		}
	}

	//�����ִ�����������
	sort(nodes.begin() + 1, nodes.end());	//�ڱ�����������

	
	//�޸�countΪ�ַ����ִ�����ǰn���
	size_t count = 0;
	for (FanoNode& node : nodes) {
		count += node.count;
		node.count = count;
	}

	//���ִ�������
	create(root, 1, (uint16_t)nodes.size()-1);

#ifdef SHOWPROCESS
	//
	show();

#endif
}
//	called in ctor 
//	TODO���ǵݹ黯�������Ч��
inline void FanoTree::create(CodeNode*& root, uint16_t first, uint16_t last)
{
	root = new CodeNode;
	if (first == last) {
		root->key = nodes[first].key;
		return;
	}
	size_t half = (nodes[first - 1].count + nodes[last].count) / 2;
	uint16_t i = first;
	while (nodes[i].count <= half)
		i++;
	if ((half - nodes[i - 1].count) >= (nodes[i].count - half) && i < last)
		i++;
	create(root->left, i, last);
	create(root->right, first, i - 1);
}
//	���浽�ļ�
inline void FanoTree::save(obitstream& bout) const	//1��ʾҶ�ӽ�㣬0��ʾ�ڵ�
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
			//�������һ��������ж���ջ����Ƿ�Ϊnull����Ϊfano�������ʣ��ڵ�����Һ��ӽ�ȫ
			s.push(p->right);
			s.push(p->left);
		}
	}
}
//	�������ļ�
inline void FanoTree::load(ibitstream& bin)
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
