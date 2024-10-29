//
// Created by pshpj on 25. 1. 23.
//

#ifndef RECVRINGBUFFER_H
#define RECVRINGBUFFER_H

#include <types.h>
#include <Container.h>
namespace psh::net_lib {

class RingBuffer {
    //버퍼 사이즈는 2의 N승꼴

public:
    explicit RingBuffer(int size):BUFFER_SIZE(size),INDEX_MASK(BUFFER_SIZE-1),_buffer(BUFFER_SIZE)
    {
        static_assert((BUFFER_SIZE & BUFFER_SIZE - 1) == 0, "Buffer size must be power of 2");
    }

    ~RingBuffer() = default;

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&) = delete;
    RingBuffer& operator=(RingBuffer&&) = delete;

    /**
     * \
     * \return 사용중인 데이터의 크기
     */
    i32 Size() const;


    /**
     * \brief 연속적으로 한 번에 넣을 수 있는 데이터의 크기
     * \return
     */
    i32 DirectEnqueueSize() const;
    /**
     * \brief
     * \return 연속적으로 한 번에 뺄 수 있는 데이터의 크기
     */
    i32 DirectDequeueSize() const;

    ui32 Peek(char* dst, const unsigned int size) const;

    /**
     * \brief deqSize만큼 버퍼에서 제거한다.
     * \param deqSize 제거할 크기
     * \return 삭제한 데이터의 크기
     */
    i32 Dequeue(int deqSize);

    [[nodiscard]] i32 GetBufferSize() const
    {
        return BUFFER_SIZE;
    }

private:
    /**
     * \brief
     * \return front의 주소
     */
    char* GetFront()
    {
        return &_buffer[GetIndex(_front)];
    }

    int GetFrontIndex() const
    {
        return GetIndex(_front);
    }
    int GetRearIndex() const
    {
        return GetIndex(_rear);
    }
    /**
     * \brief
     * \return rear의 주소
     */
    char* GetRear()
    {
        return &_buffer[GetIndex(_rear)];
    }

    /**
     * \brief
     * \return 내부 버퍼의 시작 위치
     */
    char* GetBuffer()
    {
        return &_buffer[0];
    }

    void Clear()
    {
        _front = 0;
        _rear = 0;
    }

    /**
     * \brief size만큼 front를 이동시킨다.
     * \param size 이동시킬 front의 크기
     */
    void MoveFront(const int size)
    {
        _front += size;
    }

    /**
     * \brief size만큼 rear를 이동시킨다.
     * \param size
     */
    void MoveRear(const int size)
    {
        _rear += size;
    }


    /**
     * \brief 추가로 삽입할 수 있는 크기.
     * \return
     */
    //rear+1 == front일 때 가득 찬 것이므로 1 빼야 함.
    ui32 FreeSize() const
    {
        return BUFFER_SIZE - Size() - 1;
    }

    int GetIndex(int num) const
    {
        return num & INDEX_MASK;
    }
private:


    /**
     * \brief 데이터가 들어있는 위치
     */
    volatile int _front = 0;
    /**
     * \brief 데이터 삽입할 위치. 데이터는 전까지 있음.
     */
    volatile int _rear = 0;
    const int BUFFER_SIZE;
    const int INDEX_MASK;

    Vector<char> _buffer;
};



} // psh

#endif //RECVRINGBUFFER_H
