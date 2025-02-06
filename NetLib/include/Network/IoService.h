//
// Created by pshpj on 25. 2. 7.
//

#ifndef IOSERVICE_H
#define IOSERVICE_H

#include <boost/asio.hpp>
#include <unordered_map>
#include <functional>

namespace psh::network
{
    class Session;
    using SessionFactory = std::function<std::shared_ptr<Session>(boost::asio::io_context::executor_type executor)>;

    class IoService
    {
    public:
        explicit IoService(int threads, SessionFactory factory)
            : factory_(std::move(factory)), threads_(threads) {}

        ~IoService();

        void Start();

        void Stop();

        // 새로운 세션 생성 및 비동기 연결
        std::shared_ptr<Session> ConnectSession(const std::string& host, uint16_t port, SessionFactory factory);

        // 리스너 시작
        void StartAccept(uint16_t port);

        bool IsRunning() const;

    private:
        void DoAccept();

        void StartRead(std::shared_ptr<Session> session);

        void HandleError(std::shared_ptr<Session> session,
                         const boost::system::error_code& ec);

        boost::asio::io_context ioContext_;
        std::vector<std::jthread> threadPool_;
        std::unique_ptr<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>> work_guard_;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
        SessionFactory factory_;
        std::unordered_map<std::size_t, std::shared_ptr<Session>> sessions_;
        int threads_;
        std::atomic_flag running_;
    };
}

#endif




