#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <thread>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// TracePoint structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TracePoint {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TracePoint(std::source_location location = std::source_location::current())
		: location(location), id(std::this_thread::get_id()) {
	}
	//!< instance作成時の呼び出し元の情報を取得するためのコンストラクタ.

	//* operator [copy] (default) *//

	TracePoint(const TracePoint&)            = default;
	TracePoint& operator=(const TracePoint&) = default;

	//* operator [move] (default) *//

	TracePoint(TracePoint&&)            = default;
	TracePoint& operator=(TracePoint&&) = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::thread::id id;
	std::source_location location;

	// c++23になった場合, std::stacktraceを追加する.

};
