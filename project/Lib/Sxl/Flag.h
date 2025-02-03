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
template <class _BitType, std::integral _MaskType>
class Flag {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Flag() = default;
	Flag(_MaskType mask) : mask_(mask) {}
	Flag(_BitType bit)   : mask_(static_cast<_MaskType>(bit)) {}

	~Flag() = default;

	//* operator *//

	Flag& operator=(_MaskType mask) { mask_ = mask; return *this; }
	Flag& operator=(_BitType bit) { return operator=(static_cast<_MaskType>(bit)); }

	Flag& operator|=(_MaskType mask) { mask_ |= mask; return *this; }
	Flag& operator|=(_BitType bit) { return operator|=(static_cast<_MaskType>(bit)); }

	Flag& operator&=(_MaskType mask) { mask_ &= mask; return *this; }
	Flag& operator&=(_BitType bit) { return operator&=(static_cast<_MaskType>(bit)); }

	Flag& operator^=(_MaskType mask) { mask_ ^= mask; return *this; }
	Flag& operator^=(_BitType bit) { return operator^=(static_cast<_MaskType>(bit)); }

	Flag operator|(_MaskType mask) const { return Flag(mask_ | mask); }
	Flag operator|(_BitType bit) const { return operator|(static_cast<_MaskType>(bit)); }

	Flag operator&(_MaskType mask) const { return Flag(mask_ & mask); }
	Flag operator&(_BitType bit) const { return operator&(static_cast<_MaskType>(bit)); }

	Flag operator^(_MaskType mask) const { return Flag(mask_ ^ mask); }
	Flag operator^(_BitType bit) const { return operator^(static_cast<_MaskType>(bit)); }

	//* methods *//

	bool Test(_MaskType mask) const { return (mask_ & mask) == mask; }
	bool Test(_BitType bit) const { return Test(static_cast<_MaskType>(bit)); }

	bool Any(_MaskType mask) const { return (mask_ & mask) != 0; }
	bool Any(_BitType bit) const { return Any(static_cast<_MaskType>(bit)); }

	//* getter *//

	_MaskType Get() const { return mask_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	_MaskType mask_ = NULL;

};


_SXL_NAMESPACE_END
