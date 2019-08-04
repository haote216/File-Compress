#pragma once
#include<string>
#include<iostream>
#include<algorithm>
#include<assert.h>
#include"huffman.hpp"

using namespace std;

struct CharInfo
{
	CharInfo()
		:_appearCount(0)
	{}
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo temp(*this);
		temp._appearCount = _appearCount + info._appearCount;
		return temp;
	}
	bool operator>(const CharInfo& info)
	{
		return _appearCount > info._appearCount;
	}
	bool operator!=(const CharInfo& info)
	{
		return _appearCount != info._appearCount;
	}
	bool operator==(const CharInfo& info)
	{
		return (_appearCount == info._appearCount);
	}
	
	unsigned char _ch;
	unsigned long long _appearCount;   //当前字符出现的次数
	string _strCode;    //字符对应的Huffman编码
};

class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
			_FileInfo[i]._ch = i;
	}
	void CompressFile(const string& strFilePath)
	{
		FILE* pIn = fopen(strFilePath.c_str(), "r");//转换为C格式的字符串，用来打开文件
		if (pIn == NULL)
		{
			cout << "文件路径出错" << endl;
			return;
		}

		//1.统计每个字符出现的次数
		unsigned char* pReadBuffer = new unsigned char[1024];
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
		CharInfo invalid;
		HuffmanTree<CharInfo> ht(_FileInfo, 256, invalid);

		//3.根据Huffman树获取每个字符对应的Huffman编码
		GenerateHuffmanCode(ht.GetRoot());

		//4.写解压缩时用到的信息
		string strHeadInfo;   // 头部信息
		char szCount[32];        
		string strCode;
		size_t lineNo = 0;    //行号
		for (size_t i = 0; i < 256; ++i)
		{
			if (_FileInfo[i]._appearCount)
			{
				memset(szCount, 0, sizeof(szCount) / sizeof(szCount[0]));
				strCode += _FileInfo[i]._ch;
				strCode += ',';
				_itoa_s(_FileInfo[i]._appearCount, szCount, 32, 10);
				strCode += szCount[0];
				lineNo++;
				strCode += '\n';
			}
		}
		string strLine;
		_itoa_s(lineNo, szCount, 32, 10);
		strLine = szCount[0];
		strLine += '\n';
		strHeadInfo = strLine + strCode;

		//5.用每个字符的Huffman编码重新改写文件
		FILE* pOut = fopen("2.txt", "w");
		assert(pOut);
		
		fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), pOut);
		char ch = 0;
		char count = 0;
		char* pWriteBuff = new char[1024];
		size_t pos = 0;
		//重置读取的文件指针到文件的起始位置
		fseek(pIn, 0, SEEK_SET);
		for (;;)
		{
			size_t rdSize = fread(pReadBuffer, 1, 1024, pIn);
			if (0 == rdSize)
				break;
			for (size_t rdIdx = 0; rdIdx < rdSize; ++rdIdx)
			{ 
				string& strCode = _FileInfo[pReadBuffer[rdIdx]]._strCode;
				for (size_t i = 0; i < strCode.size(); ++i)
				{
					ch <<= 1;
					if ('1' == strCode[i])
						ch |= 1;
					pos++;
					if (8 == pos)
					{
						pWriteBuff[count++] = ch;
						if (1024 == count) 
						{
							fwrite(pWriteBuff, 1, 1024, pOut);
							count = 0;
						}
						ch = 0;
						pos = 0;
					}
				}
			}
		}
		if (pos < 8)
			pWriteBuff[count++] = (ch <<(8-pos));
		fwrite(pWriteBuff, 1, count, pOut);
		delete[] pReadBuffer;
		delete[] pWriteBuff;
		fclose(pIn);
		fclose(pOut);
	}

	void UnCompressFile(const string& strFilePath)
	{
		FILE* fIn = fopen(strFilePath.c_str(), "r");
		assert(fIn);

		//1.读取有效字符总的行数
		string strInfo;
		ReadLine(fIn, strInfo);
		int lineNo = atoi(strInfo.c_str());
		for (int i = 0; i < lineNo; ++i)
		{
			strInfo = "";
			ReadLine(fIn, strInfo);
			_FileInfo[strInfo[0]]._appearCount = atoi(strInfo.c_str() + 2);
		}

		//还原Huffman树
		HuffmanTree<CharInfo> ht(_FileInfo, 256, CharInfo());

		//还原压缩文件
		FILE* fOut = fopen("3.txt", "w");
		assert(fOut);
		char* pReadBuff = new char[1024];
		char* pWriteBuff = new char[1024];
		int pos = 8;
		HuffmanTreeNode<CharInfo>* pCur = ht.GetRoot();
		int writeSize = 0;
		unsigned long long fileLen = 0;
		for (;;)
		{
			int rdSize = fread(pReadBuff, 1, 1024, fIn);
			if (0 == rdSize)
				break;

			//解压缩：1001 0110
			for (size_t rdIdx = 0; rdIdx < rdSize; ++rdIdx)
			{
				pos = 8;
				for (int i = 0; i < 8; i++)
				{
					pos--;
					if (pReadBuff[rdIdx] & (1 << pos))
						pCur = pCur->_pRight;
					else
						pCur = pCur->_pLeft;
					if (pCur->_pLeft == NULL && pCur->_pRight == NULL)
					{
						pWriteBuff[writeSize++] = pCur->_weight._ch;
						if (1024 == writeSize)
						{
							fwrite(pWriteBuff, 1, 1024, fOut);
							writeSize = 0;
						}
						pCur = ht.GetRoot();

						// 防止压缩过度 最后一个字节可能只有部分的比特位有效
						fileLen++;
						if (fileLen == pCur->_weight._appearCount)
							break;
					}
				}
			} 
		}
		fwrite(pWriteBuff, 1, writeSize, fOut);
		delete[] pReadBuff;
		delete[] pWriteBuff;
		fclose(fIn);
		fclose(fOut);
	}

private:
	//Huffman编码
	void GenerateHuffmanCode(HuffmanTreeNode<CharInfo>* pRoot)
	{
		if (pRoot == NULL)
			return;
		GenerateHuffmanCode(pRoot->_pLeft);
		GenerateHuffmanCode(pRoot->_pRight);
		if (NULL == pRoot->_pLeft && NULL == pRoot->_pRight)
		{
			HuffmanTreeNode<CharInfo>* pCur = pRoot;
			HuffmanTreeNode<CharInfo>* pParent = pRoot->_pParent;

			string& strCode = _FileInfo[pCur->_weight._ch]._strCode;
			while (pParent)
			{
				if (pCur == pParent->_pLeft)
					strCode.push_back('0');
				else
					strCode.push_back('1');
				pCur = pParent;
				pParent = pCur->_pParent;
			}
			reverse(strCode.begin(),strCode.end());
		}
	}
	void ReadLine(FILE* fIn, string & str)
	{
		while(!feof(fIn)) //不在文件末尾
		{
			char ch = fgetc(fIn);
			if ('\n' == ch)
				return;
			str += ch;
		}
	}
private:
	CharInfo _FileInfo[256];
};

void TestFileCompress()
{
	FileCompress f;
	f.CompressFile("1.txt");
	f.UnCompressFile("2.txt");
}