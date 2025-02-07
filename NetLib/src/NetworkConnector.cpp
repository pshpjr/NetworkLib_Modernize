#include "Network/NetworkConnector.h"
#include "Network/SessionManager.h"
#include <loguru.hpp>

namespace psh::network
{
    std::shared_ptr<Session> NetworkConnector::ConnectSession(
        const std::string& host,
        uint16_t port,
        SessionFactory factory)
    {
        auto session = factory(ioContext_.get_executor());

        boost::asio::ip::tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        boost::asio::async_connect(
            session->GetSocket(),
            endpoints,
            [this, session](
                const boost::system::error_code& ec,
                const boost::asio::ip::tcp::endpoint&)
            {
                if (!ec)
                {
                    sessionManager_.RegisterSession(session->GetSocket().native_handle(), session);
                    session->OnConnect();
                    details::StartRead(std::move(session));
                }
                else
                {
                    HandleError(std::move(session), ec);
                }
            });

        return session;
    }

    void NetworkConnector::HandleError(
        std::shared_ptr<Session> session,
        const boost::system::error_code& ec)
    {
        if (session->HandleError(ec))
        {
            sessionManager_.UnregisterSession(session->GetSocket().native_handle());
        }
    }
}
