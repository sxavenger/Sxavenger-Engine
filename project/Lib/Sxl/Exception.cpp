#include "Exception.h"
SXAVENGER_LIBRARY_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#ifdef _WIN64
#include <windows.h>
#endif

//* c++
#ifdef _CONSOLE
#include <iostream>
#endif
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////
// Exception class methods
////////////////////////////////////////////////////////////////////////////////////////////

NORETURN void Exception::Debugbreak() noexcept {
	__debugbreak(); //!< windows用 debug break
	// c++26から[std::breakpoint](https://cpprefjp.github.io/reference/debugging.html)を使用する
}

NORETURN void Exception::RuntimeError(const std::string& label, const std::source_location& location) {

#ifdef _CONSOLE
	//!< console有効時
	std::cout << Exception::GetLocationMessage(location) << std::endl;
	std::cout << Exception::GetLabelMessage(label)       << std::endl;
#endif

#ifdef _WIN64
	//!< win64起動時
	MessageBoxA(
		NULL,
		(Exception::GetLocationMessage(location) + "\n" + Exception::GetLabelMessage(label)).c_str(),
		"Sxavenger Library Exception",
		MB_TASKMODAL | MB_ICONHAND | MB_TOPMOST
	);
#endif 

	Exception::Debugbreak();
}

std::string Exception::GetLocationMessage(const std::source_location& location) {
	std::ostringstream message;
	message << "[location]" << "\n";
	message << " filename: " << location.file_name()     << "\n";
	message << " function: " << location.function_name() << "\n";
	message << " line:     " << location.line()          << "\n";

	return message.str();
}

std::string Exception::GetLabelMessage(const std::string& label) {
	std::ostringstream message;
	message << "[location]" << "\n";
	message << " " << label << "\n";

	return message.str();
}
