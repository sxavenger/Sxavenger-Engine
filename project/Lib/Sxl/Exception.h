#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <string>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Exception class
////////////////////////////////////////////////////////////////////////////////////////////
class Exception {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	NORETURN static void Debugbreak() noexcept;

	NORETURN static void RuntimeError(const std::string& label, const std::source_location& location = std::source_location::current());

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::string GetLocationMessage(const std::source_location& location);

	static std::string GetLabelMessage(const std::string& label);

};

SXAVENGER_LIBRARY_NAMESPACE_END
