#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <flatbuffers/flatbuffers.h>

namespace psh::network 
{
    template<typename SessionType>
    class IoService;

    struct PacketHeader {
        uint32_t size;
        uint16_t type;
    };

    class SessionBase 
    {
    public:
        virtual ~SessionBase() = default;
        virtual void OnConnect() = 0;
        virtual void OnDisconnect() = 0;
        virtual void OnReceive(uint16_t type, const uint8_t* data, size_t length) = 0;
        virtual void OnError(const boost::system::error_code& error) = 0;

    protected:
        // Send는 IoService를 통해서만 가능
        virtual void Send(uint16_t type, const flatbuffers::FlatBufferBuilder& fbb) = 0;
    };

    template<typename Derived>
    class Session : public SessionBase, 
                   public std::enable_shared_from_this<Session<Derived>> 
    {
    public:
        Session() : readBuffer_(sizeof(PacketHeader)) {}
        virtual ~Session() = default;
        
        // CRTP 패턴을 통한 실제 구현체 접근
        Derived& GetImpl() { return static_cast<Derived&>(*this); }

    protected:
        void Send(uint16_t type, const flatbuffers::FlatBufferBuilder& fbb) override {
            if (!sendInProgress_) {
                SendImpl(type, fbb);
            } else {
                pendingSends_.emplace(type, std::vector<uint8_t>(fbb.GetBufferPointer(), 
                    fbb.GetBufferPointer() + fbb.GetSize()));
            }
        }

    private:
        template<typename T>
        friend class IoService;

        boost::asio::ip::tcp::socket& GetSocket() { return socket_; }

        void SetIoContext(boost::asio::io_context& ioContext) {
            socket_ = boost::asio::ip::tcp::socket(ioContext);
        }

        void SendImpl(uint16_t type, const flatbuffers::FlatBufferBuilder& fbb) {
            PacketHeader header{static_cast<uint32_t>(fbb.GetSize()), type};
            
            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&header, sizeof(header)));
            buffers.push_back(boost::asio::buffer(fbb.GetBufferPointer(), fbb.GetSize()));
            
            sendInProgress_ = true;
            boost::asio::async_write(socket_, buffers,
                [this](const boost::system::error_code& ec, std::size_t) {
                    sendInProgress_ = false;
                    if (!ec && !pendingSends_.empty()) {
                        auto [nextType, nextData] = std::move(pendingSends_.front());
                        pendingSends_.pop();
                        flatbuffers::FlatBufferBuilder fbb;
                        fbb.PushBytes(nextData.data(), nextData.size());
                        SendImpl(nextType, fbb);
                    }
                });
        }

        boost::asio::ip::tcp::socket socket_{boost::asio::ip::tcp::socket::executor_type()};
        std::vector<uint8_t> readBuffer_;
        size_t readPos_{0};
        bool sendInProgress_{false};
        std::queue<std::pair<uint16_t, std::vector<uint8_t>>> pendingSends_;
    };
}

#endif
