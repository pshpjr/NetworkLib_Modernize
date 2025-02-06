#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include "IoService.h"
#include <memory>
#include <string>

namespace psh::network
{
    class NetworkService
    {
    public:
        explicit NetworkService(IoService& ioService);
        
        // TCP 리스너 시작
        bool StartTcpListener(const std::string& address, uint16_t port);
        
        // TCP 클라이언트 연결
        bool ConnectTcp(const std::string& address, uint16_t port);
        
        // UDP 소켓 바인딩
        bool BindUdp(const std::string& address, uint16_t port);
        
        // 비동기 송신
        void AsyncSend(const std::vector<uint8_t>& data);
        
        // 비동기 수신 콜백 설정
        void SetReceiveCallback(std::function<void(const std::vector<uint8_t>&)> callback);
        
    private:
        IoService& ioService_;
        // TCP/UDP 소켓, 세션 관리자 등을 여기에 추가
    };
}

#endif //NETWORKSERVICE_H
