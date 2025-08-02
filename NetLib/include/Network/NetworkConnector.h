#ifndef NETWORKCONNECTOR_H
#define NETWORKCONNECTOR_H

#include <string>

namespace psh::network
{
    class SessionManager;

    class NetworkConnector
    {
    public:
        explicit NetworkConnector(boost::asio::io_context& ioContext, SessionManager& sessionManager)
            : ioContext_(ioContext), sessionManager_(sessionManager) {}

        std::shared_ptr<Session> ConnectSession(
            const std::string& host,
            uint16_t port,
            SessionFactory factory);

    private:
        void HandleError(std::shared_ptr<Session> session,
                        const boost::system::error_code& ec);

        boost::asio::io_context& ioContext_;
        SessionManager& sessionManager_;
    };
}

#endif
