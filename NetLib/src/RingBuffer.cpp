//
// Created by pshpj on 25. 1. 23.
//

#include "RingBuffer.h"

#include <Utils.h>

namespace psh::network {

} // psh
int psh::net_lib::RingBuffer::Size() const
{
    const int rear = GetIndex(_rear);
    const int front = GetIndex(_front);

    if (rear >= front)
    {
        return rear - front;
    }

    return BUFFER_SIZE - (front - rear);
}

int psh::net_lib::RingBuffer::DirectEnqueueSize() const
{
    const int rear = GetIndex(_rear);
    const int front = GetIndex(_front);

    //front는 변할 수 있지만 rear는 변하지 않음.
    //가득 찬 상황
    if (GetIndex(rear + 1) == front)
    {
        return 0;
    }

    //if문 탐지할 때 front와 들어가서 front 값이 달라질 수 있음.
    //front는 증가만 하지만 rear와 front가 둘 다 버퍼 끝자락에서 있을 때 front가 증가해 rear보다 작아질 수 있음.
    //이 경우 result가 음수 나올 수 있음.
    if (rear < front)
    {
        const int result = front - rear - 1;
        AssertCrash(result > 0, "Out of Case");
        return result;
    }
    if (rear >= front)
    {
        int result;
        if (front == 0)
        {
            result = BUFFER_SIZE - rear - 1;
        }
        else
        {
            result = BUFFER_SIZE - rear;
        }

        AssertCrash(result > 0, "Out of Case");

        return result;
    }

    AssertCrash(false, "Out of Case");

    return 0;
}

int psh::net_lib::RingBuffer::DirectDequeueSize() const
{
    const int rear = GetIndex(_rear);
    const int front = GetIndex(_front);
    if (rear >= front)
    {
        return rear - front;
    }
    return BUFFER_SIZE - front;
}

unsigned int psh::net_lib::RingBuffer::Peek(char* dst, const unsigned int size) const
{
    const int front = GetIndex(_front);
    const unsigned int peekSize = size;
    //size는 늘어나기만 함.
    if (Size() < size)
    {
        throw;
    }


    if (DirectDequeueSize() >= peekSize)
    {
        memcpy_s(dst, peekSize, &_buffer[front], peekSize);
    }
    else
    {
        const int deqSize = DirectDequeueSize();

        memcpy_s(dst, deqSize, &_buffer[front], deqSize);
        memcpy_s(dst + deqSize, peekSize - deqSize, &_buffer[0], peekSize - deqSize);
    }

    return peekSize;
}

int psh::net_lib::RingBuffer::Dequeue(int deqSize)
{
    if (Size() == 0)
    {
        return 0;
    }

    //이후 dequeue 할 내용이 있다고 가정함.

    int size = deqSize;

    //size 함수는 늘어나기만 함. if 이후 변경되어도 상관 없음.
    if (deqSize > Size())
    {
        __debugbreak();
        size = Size();
    }

    _front += size;


    return size;
}
