//
// Created by pshpj on 25. 1. 24.
//

#ifndef IOSERVICE_H
#define IOSERVICE_H
#include <boost/asio/io_context.hpp>


namespace psh::network
{
    class IoService
    {
    public:

    private:
        boost::asio::io_context ioContext_;
    };
}

#endif //IOSERVICE_H
