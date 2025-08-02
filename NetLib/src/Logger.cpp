// Logger.cpp
#include "Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/async.h> // 비동기 로거를 위한 헤더

namespace psh::logger
{
    // 로거와 스레드 풀을 캡슐화하기 위한 내부 클래스
    class Logger
    {
    public:

		Logger()
		{
			logger_ = spdlog::null_logger_mt("null_logger");
		}

        // 로거 초기화
        void Init()
        {
			static std::once_flag once;
        	std::call_once(once, [this]()
        	{
        		// 비동기 로거를 위한 스레드 풀 생성
			pool_ = std::make_shared<spdlog::details::thread_pool>(8192, 1);

			// 콘솔 sink (모든 로그)
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_level(spdlog::level::trace);

			// 파일 sink (info 이상만)
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
			file_sink->set_level(spdlog::level::info);

			// 여러 sink를 모아주는 dist_sink
			auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
			dist_sink->add_sink(console_sink);
			dist_sink->add_sink(file_sink);

			// 비동기 로거 생성
			logger_ = std::make_shared<spdlog::async_logger>(
				"multi_sink",
				dist_sink,
				pool_,
				spdlog::async_overflow_policy::block
			);

			logger_->set_level(spdlog::level::trace);
			logger_->flush_on(spdlog::level::err);
			logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
        	});
        }

        // 초기화된 로거 인스턴스 반환
        std::shared_ptr<spdlog::logger> GetLogger() const
        {
            return logger_;
        }

    private:
        ~Logger()
        {
            // 프로그램 종료 시 로거 자원 정리
            if (logger_)
            {
                logger_->flush();
            }
        }

        std::shared_ptr<spdlog::logger> logger_{};
        std::shared_ptr<spdlog::details::thread_pool> pool_{};
    };

	Logger& Get()
	{
		static Logger logger;
		return logger;
	}

    // 로거 초기화 함수
    void Init()
    {
        Get().Init();
    }

    // 로거 인스턴스 반환 함수
    std::shared_ptr<spdlog::logger> GetLogger()
    {
		return Get().GetLogger();
    }

} // namespace psh::logger