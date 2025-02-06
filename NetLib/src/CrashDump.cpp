// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "CrashDump.h"

#include <atomic>
#include <basic.hpp>
#include <filesystem>
#include <iostream>
#include <loguru.hpp>
#include <string>

#ifdef _WIN32
#include "client/windows/handler/exception_handler.h"
#else
#include "client/linux/handler/exception_handler.h"
#endif


class CrashDump
{
public:
	explicit CrashDump(const std::wstring& dumpPath)
	{
		std::filesystem::path path(dumpPath);
		std::cout << path.string() << std::endl;
		GenDir(path);
#ifdef _WIN32
		handler_ = std::make_unique<google_breakpad::ExceptionHandler>(
			dumpPath, nullptr, DumpCallback, nullptr, google_breakpad::ExceptionHandler::HANDLER_ALL);

#else
		descriptor = std::make_unique<google_breakpad::MinidumpDescriptor>(path.c_str());

		handler = std::make_unique<google_breakpad::ExceptionHandler>(
			*descriptor, nullptr, DumpCallback, nullptr, true, -1);

#endif
	}

private:
#ifdef _WIN32
	static bool DumpCallback(const wchar_t* dumpPath, const wchar_t* minidumpId, void* context,
	                         EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded)
	{
		UNREFERENCED_PARAMETER(context);
		UNREFERENCED_PARAMETER(exinfo);
		UNREFERENCED_PARAMETER(assertion);

		wprintf(L"Dump path: %s\\%s.dmp\n", dumpPath, minidumpId);

		return succeeded;
	}
#else
	static bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) {
		printf("Dump path: %s\n", descriptor.path());
		return succeeded;
	}
	std::unique_ptr< google_breakpad::MinidumpDescriptor> descriptor;
#endif


	std::unique_ptr<google_breakpad::ExceptionHandler> handler_;

	static void GenDir(const std::filesystem::path& dumpPath)
	{
		// 현재 실행 디렉터리 하위에 dump 폴더가 없으면 생성
		if (!exists(dumpPath))
		{
			create_directory(dumpPath);
		}
	}
};

void psh::utils::UseDump(const std::wstring& dumpPath)
{
	// ReSharper disable once CppEntityAssignedButNoRead
	static std::unique_ptr<CrashDump> crashDump = nullptr;
	static std::atomic isRun{false};

	if (isRun.load(std::memory_order::acquire))
	{
		std::wcout << L"UseDump는 한 번만 호출 가능합니다." << std::endl;
	}
	else
	{
		crashDump = std::make_unique<CrashDump>(dumpPath);
		isRun.store(true, std::memory_order::release);
	}
}
