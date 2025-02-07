#include "Network/Session.h"
#include <loguru.hpp>
#include "Network/SendBuffer.h"

namespace psh::network
{
    //내부적으로 패킷 여러 개 들어갈 공간 만들어둬서 복사를 줄임
    constexpr size_t kMaxPackets = 8;
    constexpr size_t kBufferSize = kMaxPackets * kMaxPackets;
    //한 번에 보낼 최대 패킷 개수. 위의 공간과 무관함.
    //moodieCamel라이브러리의 bulk 함수 인터페이스는 최대 개수를 지정해 줘야 해서 추가
    constexpr size_t kMaxPacketsInSend = 100;

    enum State : char
    {
        kConnected = 1 << 0,
        kRequestDisconnect = 1 << 1,
        kDisconnected = 1 << 2,
    };

    Session::Session(const executor_t& executor) : socket_(executor)
    {
        readBuffer_.resize(kBufferSize);
        inProgressSends_.resize(kMaxPacketsInSend);
    }

    void Session::Disconnect()
    {
        state_.fetch_or(kRequestDisconnect, std::memory_order::release);
        socket_.cancel();
    }

    void Session::Send(std::shared_ptr<SendBuffer> buffer)
    {
        pendingSends_.enqueue(std::move(buffer));
        TrySend();
    }

    tcp::socket& Session::GetSocket()
    {
        return socket_;
    }


    //독립적인 접근이 보장됨.
    void Session::RealSend()
    {
        using namespace boost::asio;
        using namespace boost::system;
        std::vector<const_buffer> buffers;
        inProgressSends_.resize(kMaxPacketsInSend);
        auto inProgress = pendingSends_.try_dequeue_bulk(inProgressSends_.begin(), kMaxPacketsInSend);
        for (int i = 0; i < inProgress; i++)
        {
            buffers.push_back(buffer(inProgressSends_[i]->PacketPtr(), inProgressSends_[i]->PacketSize()));
        }

        async_write(socket_
                    , buffers
                    , bind_allocator(HandlerAllocator<int>(writeHandler_),
                                     [self = shared_from_this()](const error_code& ec, std::size_t transferred)
                                     {
                                         self->inProgressSends_.clear();
                                         auto disconnected = self->state_.load(
                                             std::memory_order_acquire);
                                         //내가 끊어서 발생하지 않은 비정상 종료라면 알림
                                         if (ec && !(disconnected & kRequestDisconnect))
                                         {
                                             LOG_F(ERROR, "Send failed: %u", ec.value());
                                             self->OnError(ec);
                                             self->OnDisconnect();
                                             return;
                                         }
                                         //정상종료
                                         if (transferred == 0)
                                         {
                                             self->OnDisconnect();
                                             return;
                                         }

                                         self->sendInProgress_.store(
                                             false, std::memory_order_release);

                                         self->TrySend();
                                     })
        );
    }

    void Session::TrySend()
    {
        if (CanSend())
        {
            RealSend();
        }
    }

    /*
     * 동시성 이슈 해결하기 위해 while 돌면서 두 번 확인하는 패턴 사용함.
     * 첫 번째 size 확인은 불필요한 exchange 줄이기 위함
     */
    bool Session::CanSend()
    {
        while (true)
        {
            if (Disconnected())
            {
                return false;
            }

            if (pendingSends_.size_approx() == 0)
            {
                return false;
            }

            if (sendInProgress_.exchange(true, std::memory_order::acq_rel) == true)
            {
                return false;
            }

            if (pendingSends_.size_approx() == 0)
            {
                sendInProgress_.store(false, std::memory_order::release);
                continue;
            }
            break;
        }
        return true;
    }

    bool Session::Disconnected()
    {
        auto disconnected = state_.load(std::memory_order_acquire);
        if (disconnected & (kRequestDisconnect | kDisconnected))
            return true;
        return false;
    }

    void Session::OnRecv(int bytes)
    {
        if (Disconnected())
        {
            return;
        }

        writePos_ += bytes;

        while (true)
        {
            if (RemainRead() < sizeof(PacketHeader))
            {
                break;
            }

            auto& packet = *reinterpret_cast<Packet*>(&readBuffer_[readPos_]);
            if (packet.header.len > kMaxPacketSize)
            {
                Disconnect();
                return;
            }

            if (RemainRead() < packet.header.len)
            {
                break;
            }
            readPos_ += sizeof(PacketHeader);
            OnReceivePacket(&readBuffer_[readPos_], packet.header.len, packet.header.type);
            readPos_ += packet.header.len;
        }

        /*
         * 남은 애들 있으면 복사. 직렬화 라이브러리 쓰면 링 버퍼를 못 써서 복사가 발생함.
         */
        auto remain = RemainRead();
        if (remain == 0)
        {
            readPos_ = writePos_ = 0;
        }
        else if (kBufferSize - writePos_ < kMaxPacketSize)
        {
            std::copy(readBuffer_.begin() + readPos_, readBuffer_.begin() + writePos_, readBuffer_.begin());
            readPos_ = 0;
            writePos_ = remain;
        }
    }

    uint16_t Session::RemainRead() const
    {
        return writePos_ - readPos_;
    }

    void details::StartRead(std::shared_ptr<Session> session)
    {
        using namespace boost::asio;
        using namespace boost::system;
        auto& socket = session->GetSocket();
        auto& buffer = session->readBuffer_;
        auto& writePos = session->writePos_;
        auto& readHandler = session->readHandler_;
        /*
        * 멤버 함수로 만들면
        * self = move(shared_from_this());로 캡처하고
        * StartRead();를 호출하면서 매 재귀마다 복사 발생
        */
        socket.async_read_some(
            boost::asio::buffer(buffer.data() + writePos, buffer.size() - writePos),
            bind_allocator(HandlerAllocator<int>(readHandler),
                           [session = std::move(session)](const error_code& ec, std::size_t length)
                           {
                               if (!ec)
                               {
                                   session->OnRecv(length);
                                   StartRead(std::move(session));
                               }
                               else
                               {
                                   session->HandleError(ec);
                               }
                           })
        );
    }

    void Session::SetOwner(IoService* ptr)
    {
        owner_ = ptr;
    }

    //에러 처리 하는 애 플래그 확인 용도. 한 명만 disconnect 처리
    //에러 처리한 애가 세션 리스트에서 삭제도 하라고 bool로 알려줌
    bool Session::HandleError(const boost::system::error_code& error)
    {
        if (state_.fetch_or(kDisconnected) & kDisconnected)
        {
            return false;
        }

        OnError(error);
        OnDisconnect();
        return true;
    }
}
