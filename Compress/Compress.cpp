#include "stdafx.h"
#ifdef  COMPRESS
#include <iostream>
#include <string>
#include "CompressTask.h"
using namespace std;

//	main function
int main(int argc,char* argv[])
{
	string file;
	//设定待压缩文件
	cout << "请选择待压缩的文件：";
	cin >> file;
	//选择压缩编码方式
	char choice;
	do {
		cout << "请选择压缩编码方式\n"
			"s -- 香农编码\n"
			"f -- 费诺编码\n"
			"h -- 哈夫曼编码\n"
			"请选择：";
		cin >> choice;
	} while (choice != 's' && choice != 'f' && choice != 'h');
	//创建压缩任务
	CompressTask task(file,choice);
	if (!task)	//创建任务失败(打开文件失败)
		return 0;

	//压缩
	//设定压缩输出文件
	cout << "请设定压缩后的文件名：";
	cin >> file;
	cout << "压缩前文件大小：" << task.size() << 'B' << endl;
	cout << "压缩后文件大小：" << task.encoded(file) << 'B' << endl;

	//解压缩
	//设定原文件和输出文件
	cout << "请选择待解压缩的文件：";
	cin >> file;
	string outfile;
	cout << "请设定解压缩后的文件名：";
	cin >> outfile;
	CompressTask::decoded(file,outfile,choice);
	system("pause");
    return 0;
}
#endif //  COMPRESS