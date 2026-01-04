#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* texture buffer
#include "BaseOffscreenTexture.h"
#include "UnorderedTexture.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// OffscreenTextureCollection class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief OffscreenTextureを管理するクラス.
class OffscreenTextureCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture);

	const DxObject::Descriptor& GetDescriptorSRV(const std::string& name) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, std::unique_ptr<BaseOffscreenTexture>> textures_;

};

SXAVENGER_ENGINE_NAMESPACE_END
