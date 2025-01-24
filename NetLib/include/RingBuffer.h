//
// Created by pshpj on 25. 1. 23.
//

#ifndef RECVRINGBUFFER_H
#define RECVRINGBUFFER_H

#include <Container.h>
#include <types.h>
#include <Utils.h>

namespace psh::net_lib
{
    class RingBuffer
    {
        //버퍼 사이즈는 2의 N승꼴

    public:
        explicit RingBuffer(int size): bufferSize_(size), indexMask_(bufferSize_ - 1), buffer_(bufferSize_)
        {
            AssertCrash((bufferSize_ & bufferSize_ - 1) == 0, "Buffer size must be power of 2");
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
        [[nodiscard]] i32 Size() const;


        /**
         * \brief 연속적으로 한 번에 넣을 수 있는 데이터의 크기
         * \return
         */
        [[nodiscard]] i32 DirectEnqueueSize() const;
        /**
         * \brief
         * \return 연속적으로 한 번에 뺄 수 있는 데이터의 크기
         */
        [[nodiscard]] i32 DirectDequeueSize() const;

        [[nodiscard]] ui32 Peek(char* dst, unsigned int size) const;

        /**
         * \brief deqSize만큼 버퍼에서 제거한다.
         * \param deqSize 제거할 크기
         * \return 삭제한 데이터의 크기
         */
        [[nodiscard]] i32 Dequeue(int deqSize);

        [[nodiscard]] i32 GetBufferSize() const
        {
            return bufferSize_;
        }

    private:
        /**
         * \brief
         * \return front의 주소
         */
        char* GetFront()
        {
            return &buffer_[GetIndex(front_)];
        }

        [[nodiscard]] int GetFrontIndex() const
        {
            return GetIndex(front_);
        }

        [[nodiscard]] int GetRearIndex() const
        {
            return GetIndex(rear_);
        }

        /**
         * \brief
         * \return rear의 주소
         */
        [[nodiscard]] char* GetRear()
        {
            return &buffer_[GetIndex(rear_)];
        }

        /**
         * \brief
         * \return 내부 버퍼의 시작 위치
         */
        [[nodiscard]] char* GetBuffer()
        {
            return &buffer_[0];
        }

        void Clear()
        {
            front_ = 0;
            rear_ = 0;
        }

        /**
         * \brief size만큼 front를 이동시킨다.
         * \param size 이동시킬 front의 크기
         */
        void MoveFront(const int size)
        {
            front_ += size;
        }

        /**
         * \brief size만큼 rear를 이동시킨다.
         * \param size
         */
        void MoveRear(const int size)
        {
            rear_ += size;
        }


        /**
         * \brief 추가로 삽입할 수 있는 크기.
         * \return
         */
        //rear+1 == front일 때 가득 찬 것이므로 1 빼야 함.
        [[nodiscard]] ui32 FreeSize() const
        {
            return bufferSize_ - Size() - 1;
        }

        [[nodiscard]] int GetIndex(int num) const
        {
            return num & indexMask_;
        }

    private:
        /**
         * \brief 데이터가 들어있는 위치
         */
        volatile int front_ = 0;
        /**
         * \brief 데이터 삽입할 위치. 데이터는 전까지 있음.
         */
        volatile int rear_ = 0;
        const int bufferSize_;
        const int indexMask_;

        Vector<char> buffer_;
    };
} // psh

#endif //RECVRINGBUFFER_H
