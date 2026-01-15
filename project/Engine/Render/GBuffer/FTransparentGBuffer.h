#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FTransparentGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FTransparentGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Accumulate,
		Revealage
	};

	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector2ui& size);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
