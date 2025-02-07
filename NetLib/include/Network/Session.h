#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <third_party/concurrentqueue.h>

#include "HandlerMemory.h"


namespace psh::network
{
    class Session;

    namespace details
    {
        /**
         * @brief 세션 recv 계속 걸어두는 함수
         * @details 멤버 함수로 만들면 재귀호출하면서 어쩔 수 없는 shared_ptr 복사가 생김
         * 지금처럼 만들면 move로 넘길 수 있음
         */
        void StartRead(std::shared_ptr<Session> session);
    }

    class SendBuffer;
    class IoService;

    using executor_t = boost::asio::io_service::executor_type;

    /**
     * @brief 컨텐츠에서 상속받아 사용
     */
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(const executor_t& executor);
        virtual ~Session() = default;

        /**
         * @brief 해당 세션 연결 끊고 싶을 때 호출
         * 호출 후 send / recv 안 옴.
         */
        void Disconnect();

        void Send(std::shared_ptr<SendBuffer> buffer);

    protected:
        virtual void OnConnect() {};

        /**
         * @brief 세션 연결 끊어질 때 1회만 호출됩니다.
         */
        virtual void OnDisconnect() {};
        /**
         * @brief 세션 연결 끊어질 때 1회만 호출됩니다.
         * @param error boost 에러 코드
         */
        virtual void OnError(const boost::system::error_code& error) {};
        virtual void OnReceivePacket(const char* data, size_t length, uint16_t type) {};

    private:
        friend void details::StartRead(std::shared_ptr<Session> session);

        void SetOwner(IoService* ptr);
        bool HandleError(const boost::system::error_code& error);
        void OnRecv(int bytes);
        friend class IoService;

        boost::asio::ip::tcp::socket& GetSocket();
        void RealSend();
        void TrySend();
        bool CanSend();
        bool Disconnected();

        uint16_t RemainRead() const;

        IoService* owner_;

        std::atomic<char> state_{0};
        boost::asio::ip::tcp::socket socket_;
        std::vector<char> readBuffer_;
        size_t readPos_{0};
        size_t writePos_{0};
        std::atomic<bool> sendInProgress_{false};
        std::vector<std::shared_ptr<SendBuffer>> inProgressSends_;
        moodycamel::ConcurrentQueue<std::shared_ptr<SendBuffer>> pendingSends_;

        HandlerMemory writeHandler_;
        HandlerMemory readHandler_;
        //concurrency::concurrent_queue<std::shared_ptr<SendBuffer>> pendingSends_;
    };
}

#endif
