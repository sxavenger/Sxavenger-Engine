#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FProcessTexture.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessBuffer final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessBuffer() = default;

	//* container option *//

	void Create(uint8_t size, const FProcessTexture::Argument& argument);

	void Reserve(uint8_t size) { textures_.reserve(size); }

	//* process option *//

	void Import(const DirectXQueueContext* context, FRenderTexture* texture);

	void Export(const DirectXQueueContext* context, FRenderTexture* texture);

	void Next(uint8_t count = 1);

	FProcessTexture& GetCurrentTexture();

	FProcessTexture& GetPreviousTexture(uint8_t previous = 1);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	std::vector<FProcessTexture> textures_;
	uint8_t currentIndex_ = 0;

};

SXAVENGER_ENGINE_NAMESPACE_END
