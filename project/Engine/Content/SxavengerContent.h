#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "TextureBuffer/OffscreenTextureCollection.h"

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	static void RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::unique_ptr<OffscreenTextureCollection> collection_;

};
