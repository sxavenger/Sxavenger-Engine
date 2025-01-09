#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* texture
#include "BaseOffscreenTexture.h"
#include "RenderTexture.h"
#include "UnorderedTexture.h"

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// OffscreenTextureCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class OffscreenTextureCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, std::unique_ptr<BaseOffscreenTexture>> textures_;

};
