#include "Network/Session.h"
#include <loguru.hpp>

namespace psh::network
{
    // PacketHeader 크기 검증
    static_assert(sizeof(PacketHeader) == 6, "PacketHeader must be 6 bytes");
    
    // 최대 패킷 크기 제한
    constexpr size_t MAX_PACKET_SIZE = 65536;
    
    // 기본 읽기 버퍼 크기
    constexpr size_t DEFAULT_READ_BUFFER_SIZE = 8192;
    
    // 패킷 헤더 유효성 검사
    bool ValidateHeader(const PacketHeader& header) 
    {
        // 패킷 크기가 0이거나 최대 크기를 초과하면 실패
        if (header.size == 0 || header.size > MAX_PACKET_SIZE) {
            LOG_F(ERROR, "Invalid packet size: %u", header.size);
            return false;
        }
        return true;
    }
}
