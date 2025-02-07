#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <memory>
#include "SessionManager.h"
#include "NetworkConnector.h"
#include "NetworkAcceptor.h"

namespace psh::network
{
    class IoService;
    class Session;
    using SessionFactory = std::function<std::shared_ptr<Session>(boost::asio::io_context::executor_type executor)>;

    class NetworkService
    {
    public:
        explicit NetworkService(int threads, SessionFactory factory);
        ~NetworkService();

        void Start();
        void Stop();
        
        std::shared_ptr<Session> ConnectSession(const std::string& host, uint16_t port, SessionFactory factory);
        void StartAccept(uint16_t port);
        bool IsRunning() const;

    private:
        std::unique_ptr<IoService> ioService_;
        SessionManager sessionManager_;
        NetworkConnector connector_;
        NetworkAcceptor acceptor_;
        SessionFactory factory_;
    };
}

#endif
