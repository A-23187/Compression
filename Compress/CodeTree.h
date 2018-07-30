#pragma once
#ifndef CODETREE
#define CODETRE
#include <stack>
#include <queue>
#include <iostream>
#include "CodeNode.h"
#include "bitstream.h"
#include "CodeTable.h"

class CodeTree {
	void destory(CodeNode *&);
protected:
	CodeNode* root;
	CodeTree() :root(nullptr) {}
	virtual ~CodeTree();
	//virtual operator CodeTable () const = 0;	//������ת��Ϊ��Ӧ�����
	virtual operator CodeTable () const;	//������ת��Ϊ��Ӧ�����
	virtual void save(obitstream&) const;	//���浽�ļ�
	virtual void load(ibitstream&);			//�������ļ�
public:
	void show();
	friend obitstream& operator<<(obitstream&, const CodeTree&);
	friend ibitstream& operator>>(ibitstream&, CodeTree&);
	friend class CompressTask;
};

//	���������ĵݹ��ӳ���
inline void CodeTree::destory(CodeNode *&node) {
	if (!node)
		return;
	if (node->left)
		destory(node->left);
	if (node->right)
		destory(node->right);
	delete node;
	node = nullptr;
}
//	������������������
inline CodeTree::~CodeTree()
{
	destory(root);
}
//	���浽�ļ���called in op<<
inline void CodeTree::save(obitstream& bout) const
{
	std::stack<CodeNode*> s;
	CodeNode* p =root;
	while (true) {
		while (p) {
			bout << 0;
			s.push(p);
			p = p->left;
		}
		bout << 1;
		while (true) {
			p = s.top();
			s.pop();
			if (p->right) {
				p = p->right;
				break;
			}
			else {
				bout << 1;
				if (!p->left)
					bout << p->key;
				if (s.empty())
					return;
			}
		}
	}
}
//	ͨ��obitstream������д�뵽�ļ���
inline obitstream& operator<<(obitstream& bout, const CodeTree& tree)
{
	if (!tree.root)
		return bout;
	tree.save(bout);
	return bout;
}
//	�������ļ���called in op>>
inline void CodeTree::load(ibitstream& bin)
{
	bool bit;
	bin >> bit;
	if (!bin)	//�ļ�Ϊ��
		return;
	root = new CodeNode;
	CodeNode* p = root;
	std::stack<CodeNode*> s;
	while (true) {
		while (!bit) {
			s.push(p);
			bin >> bit;
			if (!bit)
				p = p->left = new CodeNode;
		}
		while (true) {
			p = s.top();
			s.pop();
			bin >> bit;
			if (bit) {
				if (!p->left)
					bin >> p->key;
				if (s.empty())
					return;
			}
			else {
				p = p->right = new CodeNode;
				break;
			}
		}
	}
}
//	ͨ��ibitstream�����ļ���ȡ����
inline ibitstream& operator>>(ibitstream& bin, CodeTree& tree)
{
	tree.load(bin);
	return bin;
}
//	������ת��Ϊ��Ӧ�����
//	TODO�����Ż�
inline CodeTree::operator CodeTable() const
{
	if (!root)
		return false;

	CodeTable table;
	struct help{	//�����ṹ��
		CodeNode* node;
		unsigned data;
		uint8_t _size;
		help(CodeNode* node, unsigned data, uint8_t size)
			:node(node), data(data), _size(size) {}
	};

	std::queue<help> q;
	q.emplace(root, 0, 0);
	CodeNode* p = nullptr;
	unsigned data;
	uint8_t size;
	
	while (!q.empty()) {
		help& t = q.front();
		p = t.node;
		data = t.data;
		size = t._size;
		q.pop();
		if (p->isLeaf()) {
			table[p->key] = bitvcr(data, size);
			continue;
		}
		if (p->left)
			q.emplace(p->left, data << 1, size + 1);
		if (p->right)
			q.emplace(p->right, (data << 1) + 1, size + 1);
	}
	return table;
}
void CodeTree::show()
{
	if (!root)
		return;
	std::queue<CodeNode*> q;
	q.push(root);
	CodeNode* p = nullptr;
	while (!q.empty()) {
		p = q.front();
		q.pop();
		if (!p->left && !p->right) {
			std::cout << p->key << '\n';
			continue;
		}
		else {
			std::cout << "�ڵ�\n";
		}
		if (p->left)
			q.push(p->left);
		if (p->right)
			q.push(p->right);
	}
}
#endif