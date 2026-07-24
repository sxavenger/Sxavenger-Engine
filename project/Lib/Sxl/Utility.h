#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <type_traits>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename ...Args> requires std::is_constructible_v<T, Args...>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename ...Args> requires std::is_constructible_v<T, Args...>
std::shared_ptr<T> make_shared(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

SXAVENGER_LIBRARY_NAMESPACE_END
