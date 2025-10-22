#include "CircularArray.h"
#include "QueueCell.h"

template class CircularArray<QueueCell>;

template <class T>
int CircularArray<T>::size()
{
	int res = 0;
	if (cirArray[index_push] != NULL)
	{
		res = arraySize;
	}
	else
	{
		res = index_push - index_pop;
		if (res < 0)
			res = arraySize - index_pop + index_push;
	}
	return res;
}

template <class T>
bool CircularArray<T>::is_full(const int Reserve)
{
	bool res = false;
	if (size()+ Reserve >= arraySize)
		res = true;

	return res;
}

template <class T>
bool CircularArray<T>::is_empty()
{
	bool res = false;
	if (index_pop == index_push && cirArray[index_pop] == NULL)
		res = true;

	return res;
}

template <class T>
bool CircularArray<T>::front(T** res)
{
	*res = cirArray[index_pop];

	if (*res != NULL)
		return true;
	else
		return false;
}

template <class T>
bool CircularArray<T>::pop(bool delMemory)
{
	bool res = false;

	mut_pop.lock();
	if (cirArray[index_pop] != NULL)
	{
		if (delMemory)
			delete cirArray[index_pop];

		cirArray[index_pop] = NULL;

		index_pop = index_pop + 1;
		if (index_pop == arraySize)
		{
			index_pop = 0;
		}

		res = true;
	}
	mut_pop.unlock();

	return res;
}

template <class T>
bool CircularArray<T>::push(T* obj)
{
	bool res = false;
	mut_push.lock();

	if (cirArray[index_push] == NULL)
	{
		cirArray[index_push] = obj;

		index_push = index_push + 1;
		if (index_push == arraySize)
			index_push = 0;

		res = true;
	}
	else
	{
		obj->~T();
		delete obj;
	}

	mut_push.unlock();

	return res;
}

template <class T>
CircularArray<T>::CircularArray(int Size)
{
	cirArray = new T * [Size];

	for (int i = 0; i < Size; i++)
		cirArray[i] = NULL;

	arraySize = Size;
	index_push = 0;
	index_pop = 0;
}

template <class T>
CircularArray<T>::~CircularArray()
{
	delete[] cirArray;
}
