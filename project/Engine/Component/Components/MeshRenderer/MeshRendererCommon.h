#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshInstanceMask enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MeshInstanceMask : uint8_t {
	None       = 0,
	Shadow     = 1 << 0,

	Default    = 0xFF
};

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshRendererCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	uint8_t GetMask() const { return mask_.Get(); }

	uint8_t GetStencil() const { return stencil_; }

	void SetEnable(bool isEnable = true) { isEnable_ = isEnable; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnable_ = true;

	//* parameter *//

	Sxl::Flag<MeshInstanceMask> mask_ = MeshInstanceMask::Default;

	uint8_t stencil_ = 0;

	//* system values *//

	std::optional<int32_t> id_ = std::nullopt;

};
