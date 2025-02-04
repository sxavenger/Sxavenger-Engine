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
// Flag class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _BitType>
class Flag {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using MaskType = std::underlying_type_t<_BitType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Flag() : mask_(NULL) {}
	Flag(MaskType mask) : mask_(mask) {}
	Flag(_BitType bit)   : mask_(static_cast<MaskType>(bit)) {}

	~Flag() = default;

	//* operator *//

	Flag& operator=(MaskType mask) { mask_ = mask; return *this; }
	Flag& operator=(_BitType bit) { return operator=(static_cast<MaskType>(bit)); }

	Flag& operator|=(MaskType mask) { mask_ |= mask; return *this; }
	Flag& operator|=(_BitType bit) { return operator|=(static_cast<MaskType>(bit)); }

	Flag& operator&=(MaskType mask) { mask_ &= mask; return *this; }
	Flag& operator&=(_BitType bit) { return operator&=(static_cast<MaskType>(bit)); }

	Flag& operator^=(MaskType mask) { mask_ ^= mask; return *this; }
	Flag& operator^=(_BitType bit) { return operator^=(static_cast<MaskType>(bit)); }

	Flag operator|(MaskType mask) const { return Flag(mask_ | mask); }
	Flag operator|(_BitType bit) const { return operator|(static_cast<MaskType>(bit)); }

	Flag operator&(MaskType mask) const { return Flag(mask_ & mask); }
	Flag operator&(_BitType bit) const { return operator&(static_cast<MaskType>(bit)); }

	Flag operator^(MaskType mask) const { return Flag(mask_ ^ mask); }
	Flag operator^(_BitType bit) const { return operator^(static_cast<MaskType>(bit)); }

	//* methods *//

	bool Test(MaskType mask) const { return (mask_ & mask) == mask; }
	bool Test(_BitType bit) const { return Test(static_cast<MaskType>(bit)); }

	bool Any(MaskType mask) const { return (mask_ & mask) != 0; }
	bool Any(_BitType bit) const { return Any(static_cast<MaskType>(bit)); }

	//* getter *//

	MaskType Get() const { return mask_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MaskType mask_ = NULL;

};


_SXL_NAMESPACE_END
