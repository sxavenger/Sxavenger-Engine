#include "CrashHandler.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "StreamLogger.h"

//* lib
#include <Lib/Adapter/Time/LocalTimePoint.h>

//* windows
#include <dbghelp.h>

//-----------------------------------------------------------------------------------------
// pragma comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dbghelp.lib")

//=========================================================================================
// static variable
//=========================================================================================

const std::filesystem::path CrashHandler::kFilename = GetDumpFilename();

////////////////////////////////////////////////////////////////////////////////////////////
// CrashHandler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CrashHandler::Install() {
	SetUnhandledExceptionFilter(CrashHandler::UnhandledExceptionFilterFunc);
	StreamLogger::Log("CrashHandler | install unhandled exception filter.");
}

LONG WINAPI CrashHandler::UnhandledExceptionFilterFunc(EXCEPTION_POINTERS* exception) {
	StreamLogger::Log("CrashHandler | an unhandled exception has occurred. A minidump file will be created.");

	if (!std::filesystem::exists(kDirectory)) {
		//!< directoryが存在しない場合は作成する
		std::filesystem::create_directories(kDirectory);
	}

	const std::filesystem::path kFilepath = kDirectory / kFilename;

	HANDLE file = CreateFileW(
		kFilepath.generic_wstring().c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (file != INVALID_HANDLE_VALUE) {

		MINIDUMP_EXCEPTION_INFORMATION information = {};
		information.ThreadId          = GetCurrentThreadId();
		information.ExceptionPointers = exception;
		information.ClientPointers    = FALSE;

		const uint32_t type
			= MiniDumpWithDataSegs
			| MiniDumpWithThreadInfo
			| MiniDumpWithIndirectlyReferencedMemory;

		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			file,
			static_cast<MINIDUMP_TYPE>(type),
			&information,
			nullptr,
			nullptr
		);

		CloseHandle(file);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

std::filesystem::path CrashHandler::GetDumpFilename() {
	LocalTimePoint current = LocalTimePoint::Now();

	LocalTimePoint::Date date = current.GetDate();
	LocalTimePoint::Time time = current.GetTime();

	//!< "YYYY-MM-DD_hh-mm-ss"形式
	return std::format(
		"{:04}-{:02}-{:02}_{:02}-{:02}-{:02}.dmp",
		date.year,
		date.month,
		date.day,
		time.hour,
		time.minute,
		time.second
	);
}
