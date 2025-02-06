#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <memory>
#include <vector>

namespace psh::network 
{
    template<typename SessionType>
    class IoService;

    class SessionBase 
    {
    public:
        virtual ~SessionBase() = default;
        virtual void OnConnect() = 0;
        virtual void OnDisconnect() = 0;
        virtual void OnReceive(const std::vector<uint8_t>& data) = 0;
        virtual void OnError(const boost::system::error_code& error) = 0;
    };

    template<typename Derived>
    class Session : public SessionBase, 
                   public std::enable_shared_from_this<Session<Derived>> 
    {
    public:
        Session() = default;
        virtual ~Session() = default;

        boost::asio::ip::tcp::socket& GetSocket() { return socket_; }
        
        // CRTP 패턴을 통한 실제 구현체 접근
        Derived& GetImpl() { return static_cast<Derived&>(*this); }

    private:
        template<typename T>
        friend class IoService;

        void SetIoContext(boost::asio::io_context& ioContext) {
            socket_ = boost::asio::ip::tcp::socket(ioContext);
        }

        boost::asio::ip::tcp::socket socket_{boost::asio::ip::tcp::socket::executor_type()};
        std::vector<uint8_t> readBuffer_;
    };
}

#endif
