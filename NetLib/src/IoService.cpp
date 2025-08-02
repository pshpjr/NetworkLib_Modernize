//
// Created by pshpj on 25. 2. 7.
//

#include "Network/IoService.h"

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

}


