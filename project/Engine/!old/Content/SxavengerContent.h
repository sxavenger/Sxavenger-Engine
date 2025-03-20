#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "TextureBuffer/OffscreenTextureCollection.h"
#include "Animation/SkinningPipeline.h"

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

	//* offscreen texture option *//

	static void RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name);

	//* skinning pipeline option *//

	static void SetSkinningPipeline(const DirectXThreadContext* context);

	static void DispatchSkinning(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::unique_ptr<OffscreenTextureCollection> collection_;
	static std::unique_ptr<SkinningComputePipeline> skinningPipeline_;

};
