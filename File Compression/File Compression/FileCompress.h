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
	unsigned long long _appearCount;   //��ǰ�ַ����ֵĴ���
	string _strCode;    //�ַ���Ӧ��Huffman����
};

class FileCompress
{
public:
	void CompressFile(const string& strFilePath)
	{
		FILE* pIn = fopen(strFilePath.c_str(), "r");//ת��ΪC��ʽ���ַ������������ļ�
		if (NULL == pIn)
		{
			cout << "�ļ�·������" << endl;
			return;
		}
		//1.ͳ��ÿ���ַ����ֵĴ���
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
		//2.��CharInfoΪȨֵ����HuffmanTree
		HuffmanTree<CharInfo> ht(_FileInfo, 256);

	}
	CharInfo _FileInfo[256];
};

void TestFileCompress()
{
	FileCompress f;
	f.CompressFile("1.txt");

}