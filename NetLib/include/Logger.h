// Logger.h
#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <fmt/core.h> // spdlog가 사용하는 포맷팅 라이브러리 (직접 사용하지 않아도 되지만, 명시적으로 포함)


namespace psh::logger
{
	class Logger;

	void Init();
	std::shared_ptr<spdlog::logger> GetLogger();

	// --- 간편 로깅 함수들 ---

	// Trace 레벨 로그
	template<typename... Args>
	void Trace(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->trace(fmt, std::forward<Args>(args)...);
	}

	// Debug 레벨 로그
	template<typename... Args>
	void Debug(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->debug(fmt, std::forward<Args>(args)...);
	}

	// Info 레벨 로그
	template<typename... Args>
	void Info(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->info(fmt, std::forward<Args>(args)...);
	}

	// Warning 레벨 로그
	template<typename... Args>
	void Warn(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->warn(fmt, std::forward<Args>(args)...);
	}

	// Error 레벨 로그
	template<typename... Args>
	void Error(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->error(fmt, std::forward<Args>(args)...);
	}

	// Critical 레벨 로그
	template<typename... Args>
	void Critical(spdlog::format_string_t<Args...> fmt, Args &&... args)
	{
		GetLogger()->critical(fmt, std::forward<Args>(args)...);
	}

} // namespace psh::logger