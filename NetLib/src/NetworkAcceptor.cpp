#include "Network/NetworkAcceptor.h"
#include "Network/SessionManager.h"
#include <loguru.hpp>

namespace psh::network
{
    void NetworkAcceptor::StartAccept(uint16_t port, SessionFactory factory)
    {
        LOG_F(INFO, "Start Accept. port : %d", port);
        factory_ = std::move(factory);
        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
            ioContext_,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

        DoAccept();
    }

    void NetworkAcceptor::Stop()
    {
        if (acceptor_)
        {
            acceptor_->close();
            acceptor_.reset();
        }
    }

    void NetworkAcceptor::DoAccept()
    {
        auto session = factory_(ioContext_.get_executor());

        acceptor_->async_accept(
            session->GetSocket(),
            [this, session](const boost::system::error_code& ec)
            {
                if (!ec)
                {
                    LOG_F(INFO, "Accepted. IP : %s Port : %d",
                          session->GetSocket().remote_endpoint().address().to_string().c_str(),
                          session->GetSocket().remote_endpoint().port());
                    sessionManager_.RegisterSession(session->GetSocket().native_handle(), session);
                    session->OnConnect();
                    details::StartRead(std::move(session));
                }
                DoAccept();
            });
    }
}
