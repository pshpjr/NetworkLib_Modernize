//
// Created by pshpj on 25. 1. 24.
//

#ifndef IOSERVICE_H
#define IOSERVICE_H

#include <boost/asio/io_context.hpp>
#include "ThreadPool.h"

namespace psh::network
{
    class IoService
    {
    public:
        explicit IoService(psh::ThreadPool& threadPool);
        ~IoService();

        // io_context 실행/정지
        void Start();
        void Stop();

        // 비동기 작업 예약
        void Post(std::function<void()> task);
        void Dispatch(std::function<void()> task);

        // io_context 접근자
        boost::asio::io_context& GetIoContext() { return ioContext_; }
        const boost::asio::io_context& GetIoContext() const { return ioContext_; }

        // 상태 확인
        bool IsRunning() const { return !ioContext_.stopped(); }

    private:
        boost::asio::io_context ioContext_;
        psh::ThreadPool& threadPool_;
        bool running_{false};
    };
}

#endif //IOSERVICE_H
