//
// Created by pshpj on 25. 1. 24.
//

#ifndef CLOG_H
#define CLOG_H



namespace attrs = boost::log::attributes;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
using namespace logging::trivial;

//라인 넘버링과 해당 소스파일 출력을 첨가하기 위한 매크로
#define CUSTOM_LOG_TRACE(logger,sev) \
BOOST_LOG_SEV(logger, sev) <<"["<< sev <<"]" << "(" << __FILE__ << ", " << __LINE__ << ") " << ":"


//글로벌 로거 생성을 위한 매크로 - thread-safe를 위해 글로벌 로거 사용


class BoostLogger {

public:

	typedef sinks::text_ostream_backend backend_t;
	typedef sinks::text_file_backend file_sink;
    //비동기 싱크 자료형 선언 - ostream, Queueing 방식, 레코드 순서, 속성 값 유형, 선택적 속성 값 비교(기본적으로 std::less 사용)
	typedef sinks::asynchronous_sink<sinks::text_file_backend, sinks::unbounded_ordering_queue
		<logging::attribute_value_ordering<unsigned int, std::less<unsigned int>>>> sink_t;
	boost::shared_ptr<sink_t> sink;
	boost::shared_ptr<logging::core> core;
	BoostLogger()
	{
		try
		{
			core = logging::core::get();
			//위에서 선언한 sink_t에 맞춰 text_file_backend, RecordID 속성 값에 따른 ordering 설정
			sink = boost::make_shared<sink_t>(boost::make_shared<file_sink>(),
				keywords::order = logging::make_attr_ordering("RecordID", std::less<unsigned int>()));

			//sink backend 설정
			sink->locked_backend()->set_file_collector(sinks::file::make_collector(keywords::target = "logs"));
			sink->locked_backend()->set_rotation_size(1024*10*10);
			sink->locked_backend()->set_time_based_rotation(sinks::file::rotation_at_time_point(0, 0, 0));
			sink->locked_backend()->set_open_mode(std::ios_base::out | std::ios_base::app);
			sink->locked_backend()->set_file_name_pattern("boost_srv_%Y%m%d_%N.log");
			sink->locked_backend()->scan_for_files();

			//출력 포멧 설정
			sink->set_formatter(
				expr::stream
				<< "<" << expr::attr<boost::thread::id>("ThreadID") << ">"
				<< "[" << expr::attr<unsigned int>("RecordID") << "]"
				<< "["
				<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
				<< "]"
				<< expr::smessage
			);
		}
		catch (std::exception& e)
		{
			std::cout << "FAILURE:" << e.what() << std::endl;
		}

	}

	~BoostLogger()
	{
		core->remove_sink(sink);

		sink->stop();
		sink->flush();
		sink.reset();
	}

	boost::shared_ptr<sink_t> getSink()
	{
		return sink;
	}


};


#endif //CLOG_H
