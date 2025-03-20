#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral _Ty>
class Color3Integral; //!< integral color class template

template <std::floating_point _Ty>
class Color3floating; //!< floating-point color class template

////////////////////////////////////////////////////////////////////////////////////////////
// Color3Integral class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral _Ty>
class Color3Integral {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Color3Integral() noexcept = default;
	constexpr Color3Integral(_Ty _r, _Ty _g, _Ty _b) noexcept : r(_r), g(_g), b(_b) {};
	constexpr Color3Integral(const Color3Integral& rhs) noexcept = default;
	constexpr Color3Integral(Color3Integral&& rhs) noexcept = default;

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Color3Integral& operator=(const Color3Integral& rhs) noexcept = default;
	constexpr Color3Integral& operator=(Color3Integral&& rhs) noexcept = default;
	//!< 演算系のoperatorが必要な場合, 定義する

	//* cast
	template <class _U>
	constexpr operator Color3Integral<_U>() const noexcept {
		return { static_cast<_U>(r), static_cast<_U>(g), static_cast<_U>(b) };
	}

	template <std::floating_point _U>
	constexpr operator Color3floating<_U>() const noexcept {
		return {
			static_cast<_U>(r) / 255.0f, static_cast<_U>(g) / 255.0f, static_cast<_U>(b) / 255.0f
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

	// todo: formatter

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color3Integral class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral _Ty>
constexpr _Ty& Color3Integral<_Ty>::operator[](size_t index) noexcept {
	return (&r)[index];
}

template <std::integral _Ty>
constexpr const _Ty& Color3Integral<_Ty>::operator[](size_t index) const noexcept {
	return (&r)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// Color3floating class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point _Ty>
class Color3floating {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Color3floating() noexcept = default;
	constexpr Color3floating(_Ty _r, _Ty _g, _Ty _b) noexcept : r(_r), g(_g), b(_b) {};
	constexpr Color3floating(const Color3floating& rhs) noexcept = default;
	constexpr Color3floating(Color3floating&& rhs) noexcept = default;

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Color3floating& operator=(const Color3floating& rhs) noexcept = default;
	constexpr Color3floating& operator=(Color3floating&& rhs) noexcept = default;
	//!< 演算系のoperatorが必要な場合, 定義する

	//* cast
	template <class _U>
	constexpr operator Color3floating<_U>() const noexcept {
		return { static_cast<_U>(r), static_cast<_U>(g), static_cast<_U>(b) };
	}

	template <std::integral _U>
	constexpr operator Color3Integral<_U>() const noexcept {
		return {
			static_cast<_U>(r * 255.0f), static_cast<_U>(g * 255.0f), static_cast<_U>(b * 255.0f)
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

	// todo: formatter

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color3floating class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point _Ty>
constexpr _Ty& Color3floating<_Ty>::operator[](size_t index) noexcept {
	return (&r)[index];
}

template <std::floating_point _Ty>
constexpr const _Ty& Color3floating<_Ty>::operator[](size_t index) const noexcept {
	return (&r)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr constants
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point _Ty = float>
constexpr Color3floating<_Ty> kBlack3 = { 0, 0, 0 };

template <std::floating_point _Ty = float>
constexpr Color3floating<_Ty> kWhite3 = { 1, 1, 1 };

template <std::floating_point _Ty = float>
constexpr Color3floating<_Ty> kRed3 = { 1, 0, 0 };

template <std::floating_point _Ty = float>
constexpr Color3floating<_Ty> kGreen3 = { 0, 1, 0 };

template <std::floating_point _Ty = float>
constexpr Color3floating<_Ty> kBlue3 = { 0, 0, 1 };

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Color3i  = Color3Integral<std::int32_t>;  //!< integral color
using Color3ui = Color3Integral<std::uint32_t>; //!< unsigned integral color

using Color3f = Color3floating<float>;  //!< floating-point color
using Color3d = Color3floating<double>; //!< double precision floating-point color
