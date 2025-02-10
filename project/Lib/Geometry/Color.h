#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Sxl/Formatter.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept ColorConcept = std::integral<T> || std::floating_point<T>;

////////////////////////////////////////////////////////////////////////////////////////////
// Color3 class
////////////////////////////////////////////////////////////////////////////////////////////
template <ColorConcept T>
class Color3 {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	constexpr Color3() = default;
	constexpr Color3(T _r, T _g, T _b) : r(_r), g(_g), b(_b) {}

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
		T r, x;
	};

	union {
		T g, y;
	};

	union {
		T b, z;
	};

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(r: {}, g: {}, b: {})", r, g, b);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// Color4 class
////////////////////////////////////////////////////////////////////////////////////////////
template <ColorConcept T>
class Color4 {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	constexpr Color4() = default;
	constexpr Color4(T _r, T _g, T _b, T _a) : r(_r), g(_g), b(_b), a(_a) {}

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
		T r, x;
	};

	union {
		T g, y;
	};

	union {
		T b, z;
	};

	union {
		T a, w;
	};

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(r: {}, g: {}, b: {}, a: {})", r, g, b, a);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Color3<T>> : Sxl::BaseFormatter<Color3<T>> {};

template <class T>
struct std::formatter<Color4<T>> : Sxl::BaseFormatter<Color4<T>> {};

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

using Color3f = Color3<float>;
using Color3i = Color3<int32_t>;

using Color4f = Color4<float>;
using Color4i = Color4<int32_t>;

//-----------------------------------------------------------------------------------------
// constexpr
//-----------------------------------------------------------------------------------------

constexpr const Color3f kWhite3 = { 1.0f, 1.0f, 1.0f };
constexpr const Color3f kBlack3 = { 0.0f, 0.0f, 0.0f };

constexpr const Color4f kWhite4 = { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr const Color4f kBlack4 = { 0.0f, 0.0f, 0.0f, 1.0f };

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral _Out, std::floating_point _In>
constexpr Color3<_Out> ToColor3(const Color3<_In>& color) {
	return { static_cast<_Out>(color.r * 255.0f), static_cast<_Out>(color.g * 255.0f), static_cast<_Out>(color.b * 255.0f) };
}

template <std::integral _Out, std::floating_point _In>
constexpr Color4<_Out> ToColor4(const Color4<_In>& color) {
	return { static_cast<_Out>(color.r * 255.0f), static_cast<_Out>(color.g * 255.0f), static_cast<_Out>(color.b * 255.0f), static_cast<_Out>(color.a * 255.0f) };
}

template <std::floating_point _Out, std::integral _In>
constexpr Color3<_Out> ToColor3(const Color3<_In>& color) {
	return { static_cast<_Out>(color.r) / 255.0f, static_cast<_Out>(color.g) / 255.0f, static_cast<_Out>(color.b) / 255.0f };
}

template <std::floating_point _Out, std::integral _In>
constexpr Color4<_Out> ToColor4(const Color4<_In>& color) {
	return { static_cast<_Out>(color.r) / 255.0f, static_cast<_Out>(color.g) / 255.0f, static_cast<_Out>(color.b) / 255.0f, static_cast<_Out>(color.a) / 255.0f };
}

template <std::integral _Out, std::integral _InCode>
constexpr Color3<_Out> ToColor3(_InCode code) {
	return Color3<_Out>{
		static_cast<_Out>((code >> (8 * 2)) & 0xFF),
		static_cast<_Out>((code >> (8 * 1)) & 0xFF),
		static_cast<_Out>((code >> (8 * 0)) & 0xFF)
	};
}

template <std::integral _Out, std::integral _InCode>
constexpr Color4<_Out> ToColor4(_InCode code) {
	return Color4<_Out>{
		static_cast<_Out>((code >> (8 * 3)) & 0xFF),
		static_cast<_Out>((code >> (8 * 2)) & 0xFF),
		static_cast<_Out>((code >> (8 * 1)) & 0xFF),
		static_cast<_Out>((code >> (8 * 0)) & 0xFF)
	};
}

template <std::floating_point _Out, std::integral _InCode>
constexpr Color3<_Out> ToColor3(_InCode code) {
	return Color3<_Out>{
		static_cast<_Out>((code >> (8 * 2)) & 0xFF) / 255.0f,
		static_cast<_Out>((code >> (8 * 1)) & 0xFF) / 255.0f,
		static_cast<_Out>((code >> (8 * 0)) & 0xFF) / 255.0f
	};
}

template <std::floating_point _Out, std::integral _InCode>
constexpr Color4<_Out> ToColor4(_InCode code) {
	return Color4<_Out>{
		static_cast<_Out>((code >> (8 * 3)) & 0xFF) / 255.0f,
		static_cast<_Out>((code >> (8 * 2)) & 0xFF) / 255.0f,
		static_cast<_Out>((code >> (8 * 1)) & 0xFF) / 255.0f,
		static_cast<_Out>((code >> (8 * 0)) & 0xFF) / 255.0f
	};
}
