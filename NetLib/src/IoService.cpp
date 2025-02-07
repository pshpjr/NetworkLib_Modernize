//
// Created by pshpj on 25. 2. 7.
//

#include "Network/IoService.h"

#include <loguru.hpp>

#include "Network/Session.h"

namespace psh::network
{
    IoService::~IoService()
    {
        Stop();
    }

    void IoService::Start()
    {
        if (running_.test_and_set())
        {
            return;
        }

        workGuard_ = std::make_unique<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>>(
            ioContext_.get_executor());

        for (int i = 0; i < threads_; ++i)
        {
            threadPool_.emplace_back([this]
            {
                ioContext_.run();
            });
        }
    }

    void IoService::Stop()
    {
        if (!running_.test()) return;
        running_.clear();

        workGuard_.reset();
        ioContext_.stop();
        threadPool_.clear();
    }

    std::shared_ptr<Session> IoService::ConnectSession(const std::string& host, uint16_t port, SessionFactory factory)
    {
        auto session = factory(ioContext_.get_executor());
        session->SetOwner(this);

        boost::asio::ip::tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        boost::asio::async_connect(
            session->GetSocket(),
            endpoints,
            [this, session = std::move(session)](
            const boost::system::error_code& ec,
            const boost::asio::ip::tcp::endpoint&)
            {
                if (!ec)
                {
                    sessions_.insert({session->GetSocket().native_handle(), session});
                    session->OnConnect();
                    StartRead(std::move(session));
                }
                else
                {
                    HandleError(std::move(session), ec);
                }
            });

        return session;
    }

    void IoService::StartAccept(uint16_t port)
    {
        LOG_F(INFO, "Start Accept. port : %d", port);
        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
            ioContext_,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

        DoAccept();
        ioContext_.run();
    }

    bool IoService::IsRunning() const
    {
        return !ioContext_.stopped();
    }

    void IoService::DoAccept()
    {
        auto session = factory_(ioContext_.get_executor());

        acceptor_->async_accept(
            session->GetSocket(),
            [this, session = session](const boost::system::error_code& ec)
            {
                if (!ec)
                {
                    LOG_F(INFO, "Accepted. IP : %s Port : %d",
                          session->GetSocket().remote_endpoint().address().to_string().c_str(),
                          session->GetSocket().remote_endpoint().port());
                    sessions_.insert({session->GetSocket().native_handle(), session});
                    session->OnConnect();
                    StartRead(std::move(session));
                }
                DoAccept(); // 다음 연결 대기
            });
    }


    void IoService::HandleError(std::shared_ptr<Session> session, const boost::system::error_code& ec)
    {
        if (session->HandleError(ec))
        {
            sessions_.erase(session->GetSocket().native_handle());
        }
    }
}


