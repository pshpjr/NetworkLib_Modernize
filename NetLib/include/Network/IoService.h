//
// Created by pshpj on 25. 2. 7.
//

#ifndef IOSERVICE_H
#define IOSERVICE_H

#include <boost/asio.hpp>

namespace psh::network
{
    class Session;
    using SessionFactory = std::function<std::shared_ptr<Session>(boost::asio::io_context::executor_type executor)>;

    /**
     * @brief 비동기 네트워크 I/O 작업을 관리하는 서비스 클래스
     * 멀티스레드로 동작함.
     */
    class IoService
    {
    public:
        /**
         * @brief IoService 생성자
         * @param threads 내부 작업자 스레드 풀의 크기
         * @param factory 새로운 세션 생성을 위한 팩토리 함수
         * std::function<std::shared_ptr<Session>(boost::asio::io_context::executor_type executor)>
         *
         */
        explicit IoService(int threads, SessionFactory factory)
            : factory_(std::move(factory)), threads_(threads) {}

        ~IoService();

        /**
         * @brief 서비스 시작
         * 
         * 작업자 스레드들을 생성하고 I/O 컨텍스트 실행을 시작합니다.
         * 이미 실행 중인 경우 아무 동작도 하지 않습니다.
         */
        void Start();

        /**
         * @brief 서비스 중지
         * 
         * 모든 I/O 작업을 중단하고 작업자 스레드들을 정리합니다.
         * 진행 중인 모든 세션들이 종료됩니다.
         * TODO: 안전하게 종료
         */
        void Stop();

        /**
         * @brief 새로운 클라이언트 세션 생성 및 연결
         * @param host 대상 서버의 호스트명 또는 IP 주소
         * @param port 대상 서버의 포트 번호
         * @param factory 세션 생성을 위한 팩토리 함수
         * @return 생성된 세션의 공유 포인터
         */
        boost::asio::io_context& GetContext() { return ioContext_; }
        bool IsRunning() const { return !ioContext_.stopped(); }

    private:
        boost::asio::io_context ioContext_;
        std::vector<std::jthread> threadPool_;
        std::unique_ptr<boost::asio::executor_work_guard<
            boost::asio::io_context::executor_type>> workGuard_;
        int threads_;
        // 서비스의 실행 상태를 나타내는 atomic 플래그
        // Start()에서 설정되고 Stop()에서 해제됩니다.
        std::atomic_flag running_;
    };
}

#endif




