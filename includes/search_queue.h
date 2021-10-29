#pragma once
#include <cassert>


template<class T>
class NormalQueue{
private:
	T* buffer;
	size_t begin;
	size_t end;
	size_t capacity;
	size_t count;
public:
	NormalQueue();
	void Push(T element);
	void Pop();
	T Front();
	bool Empty();
	size_t Size();
	void Clear();
	void Reserve(size_t size);
};

template<class T, class Priority=std::greater<T>>
class PriorityQueue{
private:
	T* buffer;
	size_t count;
	size_t capacity;
	Priority priority;

	inline size_t getParent(size_t i) {
		return (i - 1) / 2;
	}
	
	inline size_t getLeft(size_t i) {
		return i * 2 + 1;
	}

	inline size_t getRight(size_t i) {
		return i * 2 + 2;
	}

	inline size_t priorityChild(size_t i) {
		size_t left = getLeft(i);
		size_t right = left + 1;
		if (right < count)return priorityElement(left, right);
		if (left < count)return left;
		return -1;
	}

	inline size_t priorityElement(size_t e1, size_t e2) {
		return priority(buffer[e2], buffer[e1]) ? e2 : e1;
	}

	inline void swap(size_t e1, size_t e2) {
		T tmp = buffer[e1];
		buffer[e1] = buffer[e2];
		buffer[e2] = tmp;
	}

public:
	PriorityQueue();
	void Push(T element);
	void Pop();
	T Front();
	bool Empty();
	size_t Size();
	void Clear();
	void Reserve(size_t size);
};

template<class T>
NormalQueue<T>::NormalQueue()
{
	begin = 0;
	end = 0;
	capacity = 0;
	buffer = nullptr;
	count = 0;
}

template<class T>
inline void NormalQueue<T>::Push(T element)
{
	if (count == capacity) {
		if (capacity == 0) {
			Reserve(32);
		}
		else {
			Reserve(capacity * 2);
		}
	}
	buffer[end] = element;
	count++;
	end = (end + 1);
	if (end >= capacity)end -= capacity;
}

template<class T>
inline void NormalQueue<T>::Pop()
{
	if (count > 0) {
		begin = (begin + 1);
		count--;
		if (count == 0)begin = end = 0;
		else if (begin >= capacity)begin -= capacity;
	}
}

template<class T>
inline T NormalQueue<T>::Front()
{
	assert(count > 0 && "Front() called on empty queue");
	return buffer[begin];
}

template<class T>
inline bool NormalQueue<T>::Empty()
{
	return count == 0;
}

template<class T>
inline size_t NormalQueue<T>::Size()
{
	return count;
}

template<class T>
inline void NormalQueue<T>::Clear()
{
	begin = 0;
	end = 0;
	count = 0;
}

template<class T>
inline void NormalQueue<T>::Reserve(size_t size)
{
	if (capacity < size) {
		T* tmp = buffer;
		buffer = new T[size];
		if (count > 0) {
			if (begin < end) {
				std::copy(tmp + begin, tmp + end, buffer);
				begin = 0;
				end = count;
			}
			else{
				std::copy(tmp + begin, tmp + capacity, buffer);
				if (end > 0)std::copy(tmp, tmp + end, buffer + (capacity - begin));
				begin = 0;
				end = count;
			}
		}
		capacity = size;
		delete tmp;
	}
}

template<class T, class Priority>
PriorityQueue<T, Priority>::PriorityQueue()
{
	buffer = nullptr;
	count = 0;
	capacity = 0;
}

template<class T, class Priority>
void PriorityQueue<T, Priority>::Push(T element)
{
	if (count == capacity) {
		if (capacity == 0) {
			Reserve(32);
		}
		else {
			Reserve(capacity * 2);
		}
	}
	buffer[count] = element;
	count++;
	size_t current = count - 1;
	size_t parent;
	while (current > 0 && priority(buffer[current], buffer[(parent = getParent(current))])) {
		swap(parent, current);
		current = parent;
	}
}

template<class T, typename Priority>
void PriorityQueue<T, Priority>::Pop()
{
	if (count > 0) {
		swap(0, count - 1);
		count--;
		size_t current = 0;
		size_t left = getLeft(current);
		size_t right = left + 1;
		while (left < count) {
			if (right < count) {
				T lv = buffer[left];
				T rv = buffer[right];
				if (priority(lv, rv)) {
					if (!priority(buffer[current], lv)) {
						swap(current, left);
						current = left;
					}else {
						break;
					}
				}
				else {
					if (!priority(buffer[current], rv)) {
						swap(current, right);
						current = right;
					}else {
						break;
					}
				}
			}else {
				if (!priority(buffer[current], buffer[left])) {
					swap(current, left);
					current = left;
				}else {
					break;
				}
			}
			left = getLeft(current);
			right = left + 1;
		}
	}
}

template<class T, class Priority>
T PriorityQueue<T, Priority>::Front()
{
	assert(count > 0 && "Front() called on empty queue");
	return buffer[0];
}

template<class T, class Priority>
inline bool PriorityQueue<T, Priority>::Empty()
{
	return count == 0;
}

template<class T, class Priority>
inline size_t PriorityQueue<T, Priority>::Size()
{
	return count;
}

template<class T, class Priority>
inline void PriorityQueue<T, Priority>::Clear()
{
	count = 0;
}

template<class T, class Priority>
inline void PriorityQueue<T, Priority>::Reserve(size_t size)
{
	if (capacity < size) {
		T* tmp = buffer;
		buffer = new T[size];
		std::copy(tmp, tmp + count, buffer);
		capacity = size;
		delete tmp;
	}
}
