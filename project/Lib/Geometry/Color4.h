#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Color3.h"

//* c++
#include <cstdint>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral _Ty>
class Color4Integral; //!< integral color class template

template <std::floating_point _Ty>
class Color4floating; //!< floating-point color class template

////////////////////////////////////////////////////////////////////////////////////////////
// Color4Integral class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral _Ty>
class Color4Integral {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Color4Integral() noexcept = default;
	constexpr Color4Integral(_Ty _r, _Ty _g, _Ty _b, _Ty _a) noexcept : r(_r), g(_g), b(_b), a(_a) {};
	constexpr Color4Integral(const Color4Integral& rhs) noexcept = default;
	constexpr Color4Integral(Color4Integral&& rhs) noexcept = default;

	constexpr Color4Integral(const Color3Integral<_Ty>& rhs, _Ty _a) noexcept : r(rhs.r), g(rhs.g), b(rhs.b), a(_a) {};

	//=========================================================================================
	// operator
	//=========================================================================================
	
	//* compound assignment
	constexpr Color4Integral& operator=(const Color4Integral& rhs) noexcept = default;
	constexpr Color4Integral& operator=(Color4Integral&& rhs) noexcept = default;
	//!< 演算系のoperatorが必要な場合, 定義する
	//! 
	//* cast
	template <class _U>
	constexpr operator Color4Integral<_U>() const noexcept {
		return { static_cast<_U>(r), static_cast<_U>(g), static_cast<_U>(b), static_cast<_U>(a) };
	}

	template <std::floating_point _U>
	constexpr operator Color4floating<_U>() const noexcept {
		return {
			static_cast<_U>(r) / 255.0f, static_cast<_U>(g) / 255.0f, static_cast<_U>(b) / 255.0f, static_cast<_U>(a) / 255.0f
		};
	}

	//* access
	constexpr _Ty& operator[](size_t index) noexcept;
	constexpr const _Ty& operator[](size_t index) const noexcept;

	//=========================================================================================
	// variables
	//=========================================================================================

	union {
		_Ty r, x;
	};

	union {
		_Ty g, y;
	};

	union {
		_Ty b, z;
	};

	union {
		_Ty a, w;
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color4Integral class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral _Ty>
constexpr _Ty& Color4Integral<_Ty>::operator[](size_t index) noexcept {
	return (&r)[index];
}

template <std::integral _Ty>
constexpr const _Ty& Color4Integral<_Ty>::operator[](size_t index) const noexcept {
	return (&r)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// Color4floating class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point _Ty>
class Color4floating {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Color4floating() noexcept = default;
	constexpr Color4floating(_Ty _r, _Ty _g, _Ty _b, _Ty _a) noexcept : r(_r), g(_g), b(_b), a(_a) {};
	constexpr Color4floating(const Color4floating& rhs) noexcept = default;
	constexpr Color4floating(Color4floating&& rhs) noexcept = default;

	constexpr Color4floating(const Color3floating<_Ty>& _c3, _Ty _a) noexcept : r(_c3.r), g(_c3.g), b(_c3.b), a(_a) {};

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Color4floating& operator=(const Color4floating& rhs) noexcept = default;
	constexpr Color4floating& operator=(Color4floating&& rhs) noexcept = default;
	//!< 演算系のoperatorが必要な場合, 定義する
	
	//* cast
	template <class _U>
	constexpr operator Color4floating<_U>() const noexcept {
		return { static_cast<_U>(r), static_cast<_U>(g), static_cast<_U>(b), static_cast<_U>(a) };
	}

	template <std::integral _U>
	constexpr operator Color4Integral<_U>() const noexcept {
		return {
			static_cast<_U>(r * 255.0f), static_cast<_U>(g * 255.0f), static_cast<_U>(b * 255.0f), static_cast<_U>(a * 255.0f)
		};
	}

	//* access
	constexpr _Ty& operator[](size_t index) noexcept;
	constexpr const _Ty& operator[](size_t index) const noexcept;

	//=========================================================================================
	// variables
	//=========================================================================================

	union {
		_Ty r, x;
	};

	union {
		_Ty g, y;
	};

	union {
		_Ty b, z;
	};

	union {
		_Ty a, w;
	};

	//=========================================================================================
	// methods
	//=========================================================================================

	static constexpr Color4floating Convert(std::uint32_t code) {
		return {
			static_cast<_Ty>((code >> 3 * 8) & 0xFF) / 255.0f,
			static_cast<_Ty>((code >> 2 * 8) & 0xFF) / 255.0f,
			static_cast<_Ty>((code >> 1 * 8) & 0xFF) / 255.0f,
			static_cast<_Ty>((code >> 0 * 8) & 0xFF) / 255.0f
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color4floating class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point _Ty>
constexpr _Ty& Color4floating<_Ty>::operator[](size_t index) noexcept {
	return (&r)[index];
}

template <std::floating_point _Ty>
constexpr const _Ty& Color4floating<_Ty>::operator[](size_t index) const noexcept {
	return (&r)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr constants
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point _Ty = float>
constexpr Color4floating<_Ty> kWhite4 = { 1, 1, 1, 1 };

template <std::floating_point _Ty = float>
constexpr Color4floating<_Ty> kBlack4 = { 0, 0, 0, 1 };

template <std::floating_point _Ty = float>
constexpr Color4floating<_Ty> kRed4 = { 1, 0, 0, 1 };

template <std::floating_point _Ty = float>
constexpr Color4floating<_Ty> kGreen4 = { 0, 1, 0, 1 };

template <std::floating_point _Ty = float>
constexpr Color4floating<_Ty> kBlue4 = { 0, 0, 1, 1 };

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Color4i  = Color4Integral<std::int32_t>; //!< integral color4
using Color4ui = Color4Integral<std::uint32_t>; //!< unsigned integral color4

using Color4f = Color4floating<float>; //!< floating-point color4
using Color4d = Color4floating<double>; //!< double precision floating-point color4

