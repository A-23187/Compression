#pragma once
#ifndef CODENODE
#define CODENODE
#include "bitstream.h"
class CodeTree;
class CodeNode {
	unsigned char key;
	CodeNode* left;
	CodeNode* right;
public:
	CodeNode() :left(nullptr), right(nullptr) {}
	CodeNode(unsigned char key) :key(key), left(nullptr), right(nullptr) {}
	bool isLeaf()const { return !left && !right; }
	friend class CodeTree;
	friend class ShannonTree;
	friend class HuffmanTree;
	friend class FanoTree;
	friend class CompressTask;
	friend obitstream& operator<<(obitstream&, const CodeTree&);
	friend ibitstream& operator>>(ibitstream&, CodeTree&);
	
};
#endif