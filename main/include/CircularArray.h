#pragma once
#include "stdafx.h"

template <class T>
class CircularArray
{
	shared_mutex mut_pop;
	shared_mutex mut_push;
	T** cirArray;
	int arraySize;
	int index_push;
	int index_pop;
	
public:
	CircularArray(int Size);
	~CircularArray();
	bool push(T* obj);
	bool front(T** res);
	bool pop(bool delMemory);
	bool is_empty();
	bool is_full(const int Reserve=0);
	int size();
};

