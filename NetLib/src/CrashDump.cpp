#include "CrashDump.h"

#include <filesystem>
#include <iostream>
#include <atomic>

#ifdef _WIN32
#include "client/windows/handler/exception_handler.h"
#else
#include "client/linux/handler/exception_handler.h"
#endif

class CrashDump
{
public:
	CrashDump(const String& dumpPath = L"")
	{
		std::filesystem::path path(dumpPath);

		GenDir(path);
#ifdef _WIN32
		handler = std::make_unique<google_breakpad::ExceptionHandler>(
			dumpPath, nullptr, DumpCallback, nullptr, google_breakpad::ExceptionHandler::HANDLER_ALL);

#else
		descriptor = std::make_unique<google_breakpad::MinidumpDescriptor>(path.c_str());

		handler = std::make_unique<google_breakpad::ExceptionHandler>(
			*descriptor, nullptr, DumpCallback, nullptr, true, -1);

#endif
	}

private:
#ifdef _WIN32
	static bool DumpCallback(const wchar_t* dump_path, const wchar_t* minidump_id, void* context,
	                         EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded)
	{
		wprintf(L"Dump path: %s\\%s.dmp\n", dump_path, minidump_id);
		return succeeded;
	}
#else
	static bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) {
		printf("Dump path: %s\n", descriptor.path());
		return succeeded;
	}
	std::unique_ptr< google_breakpad::MinidumpDescriptor> descriptor;
#endif


	std::unique_ptr<google_breakpad::ExceptionHandler> handler;

	void GenDir(const std::filesystem::path& dumpPath)
	{

		// 현재 실행 디렉터리 하위에 dump 폴더가 없으면 생성
		if (!std::filesystem::exists(dumpPath))
		{
			std::filesystem::create_directory(dumpPath);
		}
	}
};

void psh::Utils::UseDump(const String& dumpPath)
{
	static std::unique_ptr<CrashDump> crashDump = nullptr;
	static std::atomic<bool> isRun{false};

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
