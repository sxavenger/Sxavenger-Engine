#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FlagMask class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Bit, class _Mask>
class FlagMask {
	//* maskを指定してフラグを管理するクラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr FlagMask() : mask_(NULL) {}
	constexpr FlagMask(_Mask mask) : mask_(mask) {}
	constexpr FlagMask(_Bit bit) : mask_(static_cast<_Mask>(bit)) {}

	~FlagMask() = default;

	constexpr bool Test(_Mask mask) const { return (mask_ & mask) == mask; }
	constexpr bool Test(_Bit bit) const { return Test(static_cast<_Mask>(bit)); }

	constexpr bool Any(_Mask mask) const { return (mask_ & mask) != 0; }
	constexpr bool Any(_Bit bit) const { return Any(static_cast<_Mask>(bit)); }

	//* *//

	constexpr void Clear() { mask_ = NULL; }
	constexpr void Clear(_Mask mask) { mask_ &= ~mask; }

	//* getter *//

	constexpr _Mask Get() const { return mask_; }
	constexpr _Mask& Get() { return mask_; }

	//=========================================================================================
	// public operator
	//=========================================================================================

	constexpr FlagMask& operator=(_Mask mask) { mask_ = mask; return *this; }
	constexpr FlagMask& operator=(_Bit bit) { return operator=(static_cast<_Mask>(bit)); }

	constexpr FlagMask& operator|=(_Mask mask) { mask_ |= mask; return *this; }
	constexpr FlagMask& operator|=(_Bit bit) { return operator|=(static_cast<_Mask>(bit)); }

	constexpr FlagMask& operator&=(_Mask mask) { mask_ &= mask; return *this; }
	constexpr FlagMask& operator&=(_Bit bit) { return operator&=(static_cast<_Mask>(bit)); }

	constexpr FlagMask& operator^=(_Mask mask) { mask_ ^= mask; return *this; }
	constexpr FlagMask& operator^=(_Bit bit) { return operator^=(static_cast<_Mask>(bit)); }

	constexpr FlagMask operator|(_Mask mask) const { return FlagMask(mask_ | mask); }
	constexpr FlagMask operator|(_Bit bit) const { return operator|(static_cast<_Mask>(bit)); }

	constexpr FlagMask operator&(_Mask mask) const { return FlagMask(mask_ & mask); }
	constexpr FlagMask operator&(_Bit bit) const { return operator&(static_cast<_Mask>(bit)); }

	constexpr FlagMask operator^(_Mask mask) const { return FlagMask(mask_ ^ mask); }
	constexpr FlagMask operator^(_Bit bit) const { return operator^(static_cast<_Mask>(bit)); }

	constexpr const _Mask& operator&() const { return mask_; }
	constexpr _Mask& operator&() { return mask_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	_Mask mask_ = NULL;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Flag class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Bit>
class Flag
	: public FlagMask<_Bit, std::underlying_type_t<_Bit>> {
	//* enum classを指定してフラグを管理するクラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr Flag() : FlagMask<_Bit, std::underlying_type_t<_Bit>>() {}
	constexpr Flag(_Bit bit) : FlagMask<_Bit, std::underlying_type_t<_Bit>>(bit) {}

};

#define _ENUM_FLAG_OPERATORS(T) \
	inline constexpr T operator|(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) | static_cast<U>(b)); \
	} \
	inline constexpr T operator&(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) & static_cast<U>(b)); \
	} \
	inline constexpr T operator^(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b)); \
	} \
	inline constexpr T operator~(T a) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(~static_cast<U>(a)); \
	} \

_SXL_NAMESPACE_END
