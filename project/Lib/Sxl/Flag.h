#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

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

	//* check mask option  *//

	constexpr bool Test(_Mask mask) const { return (mask_ & mask) == mask; }
	constexpr bool Test(_Bit bit) const { return Test(static_cast<_Mask>(bit)); }

	constexpr bool Any(_Mask mask) const { return (mask_ & mask) != 0; }
	constexpr bool Any(_Bit bit) const { return Any(static_cast<_Mask>(bit)); }

	//* mask option *//

	constexpr void Clear() { mask_ = NULL; }
	constexpr void Clear(_Mask mask) { mask_ &= ~mask; }

	constexpr void Inverse(_Mask mask) { mask_ ^= mask; }
	constexpr void Inverse(_Bit bit) { Inverse(static_cast<_Mask>(bit)); }

	//* set option *//

	constexpr void Set(_Mask mask) { mask_ |= mask; }
	constexpr void Set(_Bit bit) { Set(static_cast<_Mask>(bit)); }

	constexpr void Reset(_Mask mask) { mask_ &= ~mask; }
	constexpr void Reset(_Bit bit) { Reset(static_cast<_Mask>(bit)); }

	//* operator [assign] *//

	constexpr FlagMask(const FlagMask&) noexcept            = default;
	constexpr FlagMask& operator=(const FlagMask&) noexcept = default;

	constexpr FlagMask(_Mask mask) noexcept : mask_(mask) {}
	constexpr FlagMask& operator=(_Mask mask) noexcept { mask_ = mask; return *this; }

	constexpr FlagMask(_Bit bit) noexcept : mask_(static_cast<_Mask>(bit)) {}
	constexpr FlagMask& operator=(_Bit bit) noexcept { mask_ = static_cast<_Mask>(bit); return *this; }

	//* operator [equal] *//

	constexpr bool operator==(_Mask mask) const noexcept { return mask_ == mask; }
	constexpr bool operator==(_Bit bit) const noexcept { return mask_ == static_cast<_Mask>(bit); }
	constexpr bool operator==(FlagMask other) const noexcept { return mask_ == other.mask_; }

	constexpr bool operator!=(_Mask mask) const noexcept { return mask_ != mask; }
	constexpr bool operator!=(_Bit bit) const noexcept { return mask_ != static_cast<_Mask>(bit); }
	constexpr bool operator!=(FlagMask other) const noexcept { return mask_ != other.mask_; }

	//* operator [or] *//

	constexpr FlagMask operator|(_Mask mask) const noexcept { return FlagMask(mask_ | mask); }
	constexpr FlagMask operator|(_Bit bit) const noexcept { return FlagMask(mask_ | static_cast<_Mask>(bit)); }
	constexpr FlagMask operator|(FlagMask other) const noexcept { return FlagMask(mask_ | other.mask_); }

	constexpr FlagMask& operator|=(_Mask mask) noexcept { mask_ |= mask; return *this; }
	constexpr FlagMask& operator|=(_Bit bit) noexcept { mask_ |= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator|=(FlagMask other) noexcept { mask_ |= other.mask_; return *this; }

	//* operator [and] *//

	constexpr FlagMask operator&(_Mask mask) const noexcept { return FlagMask(mask_ & mask); }
	constexpr FlagMask operator&(_Bit bit) const noexcept { return FlagMask(mask_ & static_cast<_Mask>(bit)); }
	constexpr FlagMask operator&(FlagMask other) const noexcept { return FlagMask(mask_ & other.mask_); }

	constexpr FlagMask& operator&=(_Mask mask) noexcept { mask_ &= mask; return *this; }
	constexpr FlagMask& operator&=(_Bit bit) noexcept { mask_ &= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator&=(FlagMask other) noexcept { mask_ &= other.mask_; return *this; }

	//* operator [xor] *//

	constexpr FlagMask operator^(_Mask mask) const noexcept { return FlagMask(mask_ ^ mask); }
	constexpr FlagMask operator^(_Bit bit) const noexcept { return FlagMask(mask_ ^ static_cast<_Mask>(bit)); }
	constexpr FlagMask operator^(FlagMask other) const noexcept { return FlagMask(mask_ ^ other.mask_); }

	constexpr FlagMask& operator^=(_Mask mask) noexcept { mask_ ^= mask; return *this; }
	constexpr FlagMask& operator^=(_Bit bit) noexcept { mask_ ^= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator^=(FlagMask other) noexcept { mask_ ^= other.mask_; return *this; }

	//* operator [cast] *//

	constexpr explicit operator _Mask() const { return mask_; }
	constexpr explicit operator _Bit() const { return static_cast<_Bit>(mask_); }

	//* getter *//

	constexpr _Mask Get() const { return mask_; }
	constexpr _Mask& Get() { return mask_; }

	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	_Mask mask_ = NULL;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Flag class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Bit> requires std::is_enum_v<_Bit>
using Flag = FlagMask<_Bit, std::underlying_type_t<_Bit>>;

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

SXAVENGER_LIBRARY_NAMESPACE_END
