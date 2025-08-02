#ifndef NETWORKACCEPTOR_H
#define NETWORKACCEPTOR_H


#include "IoService.h"
namespace psh::network
{
    class SessionManager;

    class NetworkAcceptor
    {
    public:
        explicit NetworkAcceptor(boost::asio::io_context& ioContext, SessionManager& sessionManager)
            : ioContext_(ioContext), sessionManager_(sessionManager) {}

        void StartAccept(uint16_t port, SessionFactory factory);
        void Stop();

    private:
        void DoAccept();

        boost::asio::io_context& ioContext_;
        SessionManager& sessionManager_;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
        SessionFactory factory_;
    };
}

#endif
