#pragma once
#ifndef FANOTREE
#define FANOTREE
#include <vector>
#include <algorithm>
#include "CodeTree.h"
//	辅助类 FanoNode
class FanoNode {
	unsigned char key;
	size_t count;	//前n项和
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

//	由统计得到的信息（每个字符出现次数）来构造FanoTree
inline FanoTree::FanoTree(unsigned c[])
{
	//构建nodes，这里count暂为字符出现次数
	nodes.reserve(SIZE);
	nodes.emplace_back(0, 0);	//技巧？添加前置哨兵
	for (size_t i = 0; i < SIZE; i++) {
		if (c[i]) {
			nodes.emplace_back(unsigned char(i), c[i]);
		}
	}

	//按出现次数递增排序
	sort(nodes.begin() + 1, nodes.end());	//哨兵不参与排序

	
	//修改count为字符出现次数的前n项和
	size_t count = 0;
	for (FanoNode& node : nodes) {
		count += node.count;
		node.count = count;
	}

	//二分创建码树
	create(root, 1, (uint16_t)nodes.size()-1);

#ifdef SHOWPROCESS
	//
	show();

#endif
}
//	called in ctor 
//	TODO：非递归化，以提高效率
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
//	保存到文件
inline void FanoTree::save(obitstream& bout) const	//1表示叶子结点，0表示内点
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
			//这里和下一句均不用判断入栈结点是否为null，因为fano树的性质：内点皆左右孩子健全
			s.push(p->right);
			s.push(p->left);
		}
	}
}
//	加载自文件
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
