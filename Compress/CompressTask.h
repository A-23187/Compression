#pragma once
#ifndef COMPRESSTASK
#define COMPRESSTASK
#include <cstdint>
#include <string>
#include <fstream>
#include "bitstream.h"
#include "CodeTree.h"
#include "ShannonTree.h"
#include "HuffmanTree.h"
#include "FanoTree.h"
class CompressTask {
	size_t count = 0;				//���ַ�������ѹ���ļ����ֻ����4G
	unsigned c[256] = { 0 };		//ÿ���ַ����ֵĴ���
	unsigned char* src = nullptr;	//��ѹ������
	CodeTree* tree = nullptr;					//��ѹ�����롢��ѹ��ʱ���������
	uint8_t type = 2;						//��������
	static constexpr char hz[3][5]{ ".san",".fno",".hfm" };
public:
	CompressTask() = default;
	CompressTask(const std::string&, char = 'h');
	~CompressTask();
	size_t encoded(const std::string&) const;
	static void decoded(const std::string&, const std::string&,char method = 'h');
	operator bool() const;
	size_t size() const;
	CompressTask(const CompressTask&) = default;			//��ֹcopy ctor
	CompressTask& operator=(const CompressTask&) = delete;	//��ֹcopy op=
	static size_t getFileSize(const std::string&);
};


//	ctor
inline CompressTask::CompressTask(const std::string& srcFilepath,char method)
{
	std::ifstream fin(srcFilepath, std::ios::binary);
	if (!fin)
		return;	//���ļ�ʧ��ʱ�˳�
	count = getFileSize(srcFilepath);
	if (!count) return;	//�ļ�Ϊ��ʱ�˳�
	src = new unsigned char[count];
	unsigned char buf;
	size_t i = 0;
	while (fin.read((char*)&buf, sizeof buf)) {
		src[i++] = buf;
		c[buf]++;
	}
	fin.close();

	//����method��������
	switch (method) {
	case 's':
		type = 0;
		tree = new ShannonTree(c, count);
		break;
	case 'f':
		type = 1;
		tree = new FanoTree(c);
		break;
	default:
		tree = new HuffmanTree(c);
	}
	
}
//	dtor
inline CompressTask::~CompressTask() {
	delete tree;
	delete[] src;
}
//	ѹ��ʱ����
inline size_t CompressTask::encoded(const std::string& desFilename) const
{
	if (!src)
		return 0;	//����0��ʾѹ��ʧ��
	obitstream bout((desFilename + hz[type]).c_str());
	CodeNode *p= tree->root,*pre = nullptr;

	//����ѹ����ʽ normal or fix
	int8_t len = -1;
	while (p) {
		pre = p;
		p = p->right;
		len++;
	}
	if (!pre->left && len < 8) {
		bout << unsigned char(0);
		bout.write(&count, 4);
	}
	else bout << 1;

	bout << *tree;
	CodeTable table = *tree;
	if (!table) {
		bout.close();
		return 0;	
	}
#ifdef SHOWPROCESS
#include <iostream>
	for (int i = 0; i < SIZE; i++) {
		if (table[i].empty())
			continue;
		std::cout << (unsigned char)i << ' ' << table[i] << std::endl;
	}
#endif
	for (size_t i = 0; i < count; i++) {
		bout << table[src[i]];
	}
	bout << obitstream::end1;
	bout.close();
	return getFileSize((desFilename + hz[type]));
}
//	��ѹ��ʱ����
inline void CompressTask::decoded(const std::string& infile,const std::string& outfile,char method)
{
	ibitstream bin(infile.c_str());
	bool tag;
	bin >> tag;
	size_t count;
	if (!tag) {
		unsigned char buf;
		bin.read(&count,sizeof count,buf);
	}
	CodeTree* tree;
	switch (method)
	{
	case 's':
		tree = new CodeTree;
		break;
	case 'f':
		tree = new FanoTree;
		break;
	default:
		tree = new HuffmanTree;
	}
	bin >> *tree;
	std::ofstream fout(outfile.c_str(),std::ios::binary);
	CodeNode* p = tree->root;
	bool bit;
	if(tag)
		while (bin >> bit) {
			if (bit)
				p = p->right;
			else
				p = p->left;
			if (!p)
				break;
			if (!p->left && !p->right) {
				fout.write((char*)&p->key, 1);
				p = tree->root;
			}
		}
	else while (count > 0) {
			bin >> bit;
			if (bit)
				p = p->right;
			else
				p = p->left;
			if (!p->left && !p->right) {
				fout.write((char*)&p->key, 1);
				p = tree->root;
				count--;
			}
	}
	fout.close();
}
//	�Ƿ�ɹ�����
inline CompressTask::operator bool() const
{
	return src != nullptr;
}
//	����ԭ�ļ���С
inline size_t CompressTask::size() const
{
	return count;
}
//	�����ļ���С�������ļ�ʧ��ʱ������ UINT32_MAx = -1����������Ϊ[0��UINT32_MAX)
inline size_t CompressTask::getFileSize(const std::string& filepath)
{
	FILE* fp = nullptr;
	if (!fopen_s(&fp, filepath.c_str(), "r")) {
		fseek(fp, 0, SEEK_END);
		return ftell(fp);
	}
	return UINT32_MAX;
}
#endif