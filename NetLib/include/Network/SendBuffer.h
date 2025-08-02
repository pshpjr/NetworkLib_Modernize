//
// Created by pshpj on 25. 2. 7.
//

#ifndef SENDBUFFER_H
#define SENDBUFFER_H
#include <array>
#include <memory>

namespace psh::network
{
    //단일 패킷의 최대 길이
    static constexpr int kMaxPacketSize = 1024;

#pragma pack(push, 1)
    struct PacketHeader
    {
        uint16_t len{};
        uint16_t type{};
    };

    struct Packet
    {
        PacketHeader header{};
        std::array<char, kMaxPacketSize> data{};
    };
#pragma pack(pop)

    class SendBuffer
    {
        friend class Session;

    public:
        /**
         * @brief 버퍼에 직렬화 한 후 호출. 1번만 호출해야 함.
         * @param type 패킷 타입
         * @param bytes 추가한 패킷 크기
         */
        void Seal(uint16_t type, int bytes);


        /**
         * @brief
         * @return 패킷 들어가는 버퍼 위치 리턴
         */
        std::array<char, kMaxPacketSize>& GetBuffer();

        /**
         * @brief
         * @return 버퍼의 최대 크기
         */
        static uint16_t Capacity();
        /**
         * @brief
         * @return 버퍼가 사용중인 크기
         */
        uint16_t Size() const;

        /**
         * @brief 버퍼 할당 받을 때 이 함수만 쓰세요
         */
        static std::shared_ptr<SendBuffer> Alloc();

    private:
        SendBuffer() = default;

        /**
         * @brief 헤더 포함 크기. 라이브러리 내부에서 사용
         */
        uint16_t PacketSize() const;
        /**
         * @brief 헤더 포함한 패킷 포인터. 내부적으로 사용
         */
        uint8_t* PacketPtr();

        Packet buffer_;
    	int bufferSize_ = kMaxPacketSize;
        int dataSize_ = 0;
    };
}


#endif //SENDBUFFER_H
