//
// Created by pshpj on 25. 2. 7.
//

#include "SendBuffer.h"

namespace psh::network
{
    void SendBuffer::Seal(uint16_t type, int bytes)
    {
        buffer_.header.len = bytes + sizeof(Packet::header);
        buffer_.header.type = type;
        dataSize_ = bytes;
    }

    std::array<char, kMaxPacketSize>& SendBuffer::GetBuffer()
    {
        return buffer_.data;
    }

    uint16_t SendBuffer::Capacity()
    {
        return kMaxPacketSize;
    }

    uint16_t SendBuffer::Size() const
    {
        return dataSize_;
    }

    std::shared_ptr<SendBuffer> SendBuffer::Alloc()
    {
		if (auto pBuf = new SendBuffer(); pBuf)
		{
			return std::shared_ptr<SendBuffer>(pBuf);
		}
		throw std::bad_alloc();
    	return nullptr;
    }

    uint16_t SendBuffer::PacketSize() const
    {
        return buffer_.header.len;
    }

    uint8_t* SendBuffer::PacketPtr()
    {
        return reinterpret_cast<uint8_t*>(&buffer_);
    }
};
