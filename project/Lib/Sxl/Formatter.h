#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

template <class T>
struct BaseFormatter {
public:

	// デフォルトのフォーマットロジック
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	// 書式設定のロジックを派生クラスがカスタマイズ
	template <typename FormatContext>
	auto format(const T& value, FormatContext& ctx) const {
		return value.format(ctx); // 派生クラスの format メソッドを呼び出す
	}

};

_SXL_NAMESPACE_END