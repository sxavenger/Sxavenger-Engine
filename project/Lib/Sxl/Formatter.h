#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseFormatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct BaseFormatter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	// デフォルトのフォーマットロジック
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	// 書式設定のロジックを派生クラスがカスタマイズ
	template <typename FormatContext>
	auto format(const T& value, FormatContext& ctx) const {
		return value.Format(ctx); // 派生クラスの format メソッドを呼び出す
	}

};

SXAVENGER_LIBRARY_NAMESPACE_END
