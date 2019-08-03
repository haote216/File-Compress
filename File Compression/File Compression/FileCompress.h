#pragma once
#include<string>
#include<iostream>
#include"huffman.hpp"
using namespace std;

struct CharInfo
{
	CharInfo()
		:_appearCount(0)
	{}

	char _ch;
	unsigned long long _appearCount;   //当前字符出现的次数
	string _strCode;    //字符对应的Huffman编码
};

class FileCompress
{
public:
	void CompressFile(const string& strFilePath)
	{
		FILE* pIn = fopen(strFilePath.c_str(), "r");//转换为C格式的字符串，用来打开文件
		if (NULL == pIn)
		{
			cout << "文件路径出错" << endl;
			return;
		}
		//1.统计每个字符出现的次数
		char* pReadBuffer = new char[1024];
		for (;;)
		{
			int rdSize = fread(pReadBuffer, 1, 1024, pIn);
			if (rdSize == 0)
				break;
			for (size_t i = 0; i < rdSize; ++i)
			{
				_FileInfo[pReadBuffer[i]]._appearCount++;
			}
		}
		//2.以CharInfo为权值创建HuffmanTree
		HuffmanTree<CharInfo> ht(_FileInfo, 256);

	}
	CharInfo _FileInfo[256];
};

void TestFileCompress()
{
	FileCompress f;
	f.CompressFile("1.txt");

}