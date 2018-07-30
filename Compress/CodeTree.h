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
	//virtual operator CodeTable () const = 0;	//将码树转换为对应的码表
	virtual operator CodeTable () const;	//将码树转换为对应的码表
	virtual void save(obitstream&) const;	//保存到文件
	virtual void load(ibitstream&);			//加载自文件
public:
	void show();
	friend obitstream& operator<<(obitstream&, const CodeTree&);
	friend ibitstream& operator>>(ibitstream&, CodeTree&);
	friend class CompressTask;
};

//	销毁码树的递归子程序
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
//	析构函数，销毁码树
inline CodeTree::~CodeTree()
{
	destory(root);
}
//	保存到文件，called in op<<
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
//	通过obitstream将码树写入到文件内
inline obitstream& operator<<(obitstream& bout, const CodeTree& tree)
{
	if (!tree.root)
		return bout;
	tree.save(bout);
	return bout;
}
//	加载自文件，called in op>>
inline void CodeTree::load(ibitstream& bin)
{
	bool bit;
	bin >> bit;
	if (!bin)	//文件为空
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
//	通过ibitstream，从文件读取码树
inline ibitstream& operator>>(ibitstream& bin, CodeTree& tree)
{
	tree.load(bin);
	return bin;
}
//	由码树转换为对应的码表
//	TODO：可优化
inline CodeTree::operator CodeTable() const
{
	if (!root)
		return false;

	CodeTable table;
	struct help{	//辅助结构体
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
			std::cout << "内点\n";
		}
		if (p->left)
			q.push(p->left);
		if (p->right)
			q.push(p->right);
	}
}
#endif