#pragma once
#include "MathUtil.h"

template <typename T>
class CircularBuffer
{
private:
    size_t head = 0;
    size_t tail = 0;
    const size_t capacity;
    std::unique_ptr<T[]> buf;
public:
    CircularBuffer(size_t size) :
    capacity(size),
    buf(std::unique_ptr<T[]>(new T[capacity]))
    {
    }
    
    void push(T item)
    {
        buf[tail] = item;
        tail = (tail + 1) % capacity;
        head = (head + 1) % capacity;
    }

    T& operator[] (size_t idx)
    {
        return buf[(head + idx) % capacity];
    }
};
