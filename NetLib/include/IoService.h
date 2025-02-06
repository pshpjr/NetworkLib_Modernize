#ifndef IOSERVICE_H
#define IOSERVICE_H

#include <boost/asio.hpp>
#include "Network/Session.h"
#include <unordered_map>
#include <functional>

namespace psh::network 
{
    template<typename SessionType>
    class IoService 
    {
    public:
        explicit IoService(int threads)
            : threads_(threads) {}

        ~IoService() {
            Stop();
        }

        void Start() {
            if (running_) return;
            running_ = true;

            work_guard_ = std::make_unique<boost::asio::executor_work_guard<
                boost::asio::io_context::executor_type>>(
                    ioContext_.get_executor());

            for (int i = 0; i < threads_; ++i) {
                threadPool_.emplace_back([this] { ioContext_.run(); });
            }
        }

        void Stop() {
            if (!running_) return;
            running_ = false;

            work_guard_.reset();
            ioContext_.stop();
            
            for (auto& thread : threadPool_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threadPool_.clear();
        }

        // 새로운 세션 생성 및 비동기 연결
        std::shared_ptr<SessionType> ConnectSession(
            const std::string& host, 
            uint16_t port,
            std::function<void(std::shared_ptr<SessionType>)> onConnected = nullptr)
        {
            auto session = std::make_shared<SessionType>();
            session->SetIoContext(ioContext_);
            
            boost::asio::ip::tcp::resolver resolver(ioContext_);
            auto endpoints = resolver.resolve(host, std::to_string(port));

            boost::asio::async_connect(
                session->GetSocket(),
                endpoints,
                [this, session, onConnected](
                    const boost::system::error_code& ec,
                    const boost::asio::ip::tcp::endpoint&)
                {
                    if (!ec) {
                        sessions_.insert({session->GetSocket().native_handle(), session});
                        session->OnConnect();
                        if (onConnected) {
                            onConnected(session);
                        }
                        StartRead(session);
                    } else {
                        session->OnError(ec);
                    }
                });

            return session;
        }

        // 리스너 시작
        void StartAccept(uint16_t port) {
            acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
                ioContext_, 
                boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
            
            DoAccept();
        }

        bool IsRunning() const { return !ioContext_.stopped(); }

    private:
        void DoAccept() {
            auto session = std::make_shared<SessionType>();
            session->SetIoContext(ioContext_);
            
            acceptor_->async_accept(
                session->GetSocket(),
                [this, session](const boost::system::error_code& ec)
                {
                    if (!ec) {
                        sessions_.insert({session->GetSocket().native_handle(), session});
                        session->OnConnect();
                        StartRead(session);
                    }
                    DoAccept();  // 다음 연결 대기
                });
        }

        void StartRead(std::shared_ptr<SessionType> session) {
            auto& socket = session->GetSocket();
            auto& buffer = session->readBuffer_;
            auto& readPos = session->readPos_;

            // 먼저 헤더를 읽음
            if (readPos < sizeof(PacketHeader)) {
                socket.async_read_some(
                    boost::asio::buffer(buffer.data() + readPos, 
                                      sizeof(PacketHeader) - readPos),
                    [this, session](const boost::system::error_code& ec, std::size_t length)
                    {
                        if (!ec) {
                            session->readPos_ += length;
                            if (session->readPos_ >= sizeof(PacketHeader)) {
                                // 헤더를 다 읽었으면 패킷 크기만큼 버퍼 확장
                                auto header = reinterpret_cast<PacketHeader*>(session->readBuffer_.data());
                                session->readBuffer_.resize(sizeof(PacketHeader) + header->size);
                            }
                            StartRead(session);
                        } else {
                            HandleError(session, ec);
                        }
                    });
            }
            // 헤더를 다 읽었으면 나머지 데이터를 읽음
            else {
                auto header = reinterpret_cast<PacketHeader*>(buffer.data());
                auto totalSize = sizeof(PacketHeader) + header->size;
                
                socket.async_read_some(
                    boost::asio::buffer(buffer.data() + readPos, 
                                      totalSize - readPos),
                    [this, session](const boost::system::error_code& ec, std::size_t length)
                    {
                        if (!ec) {
                            session->readPos_ += length;
                            auto& buffer = session->readBuffer_;
                            auto header = reinterpret_cast<PacketHeader*>(buffer.data());
                            auto totalSize = sizeof(PacketHeader) + header->size;

                            // 패킷을 모두 읽었으면 처리
                            if (session->readPos_ >= totalSize) {
                                session->OnReceive(header->type,
                                    buffer.data() + sizeof(PacketHeader),
                                    header->size);
                                
                                // 다음 패킷을 위해 버퍼 초기화
                                session->readPos_ = 0;
                                buffer.resize(sizeof(PacketHeader));
                            }
                            StartRead(session);
                        } else {
                            HandleError(session, ec);
                        }
                    });
            }
        }

        void HandleError(std::shared_ptr<SessionType> session,
                        const boost::system::error_code& ec) {
            session->OnError(ec);
            sessions_.erase(session->GetSocket().native_handle());
            session->OnDisconnect();
        }

        boost::asio::io_context ioContext_;
        std::vector<std::jthread> threadPool_;
        std::unique_ptr<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>> work_guard_;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
        std::unordered_map<std::size_t, std::shared_ptr<SessionType>> sessions_;
        int threads_;
        bool running_{false};
    };
}

#endif
