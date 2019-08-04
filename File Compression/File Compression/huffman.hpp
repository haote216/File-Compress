#pragma once
#include<iostream>
#include<queue>
#include<vector>
using namespace std;

template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const W& weight = W())
	:_pLeft(0)
	, _pRight(0)
	, _pParent(0)
	, _weight(weight)
	{}
	HuffmanTreeNode<W>* _pLeft;
	HuffmanTreeNode<W>* _pRight;
	HuffmanTreeNode<W>* _pParent;
	W _weight;
};

//仿函数    比较器作用：返回比较数的值。如果没有比较器，优先级队列比较的是俩个地址
template<class W>
class Compare
{
public:
	bool operator()(HuffmanTreeNode<W>* pLeft,HuffmanTreeNode<W>* pRight)
	{
		return pLeft->_weight > pRight->_weight ? true : false;	
	}
};

template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W>* pNode;
public:
	HuffmanTree(W* array, size_t size, const W& invalid)
	{
		//优先级队列(堆)q
		priority_queue<pNode, vector<pNode>, Compare<W>> q;    

		for (size_t i = 0; i < size; ++i)
		{
			if (array[i] != invalid)
				q.push(new HuffmanTreeNode<W>(array[i]));  //把结点插入堆中
		}	
		while (q.size() > 1)
		{
			HuffmanTreeNode<W>* pLeft = q.top();
			q.pop();
			HuffmanTreeNode<W>* pRight = q.top();
			q.pop();

			HuffmanTreeNode<W>* pParent = new HuffmanTreeNode<W>(pLeft->_weight + pRight->_weight);
			pParent->_pLeft = pLeft;
			pParent->_pRight = pRight;
			pLeft->_pParent = pParent;
			pRight->_pParent = pParent;
			q.push(pParent);
		}
		_pRoot =  q.top();
	}

	HuffmanTreeNode<W>* GetRoot()
	{
		return _pRoot;
	}

	~HuffmanTree()
	{
		_Destroy(_pRoot);
	}

private:
	void _Destroy(HuffmanTreeNode<W>*& pRoot)
	{
		if (pRoot)
		{
			_Destroy(pRoot->_pLeft);
			_Destroy(pRoot->_pRight);
			delete pRoot;
			pRoot = 0;
		}
	}

private:
	HuffmanTreeNode<W>* _pRoot;
};