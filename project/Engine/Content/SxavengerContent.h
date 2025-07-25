#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "TextureBuffer/OffscreenTextureCollection.h"
#include "Animation/SkinningPipeline.h"
#include "DebugPrimitive/DebugPrimitive.h"

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

	static const DxObject::Descriptor& GetDescriptorSRV(const std::string& name);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name);

	//* skinning pipeline option *//

	static void SetSkinningPipeline(const DirectXQueueContext* context);

	static void DispatchSkinning(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

	//* debug primitive option *//

	static void ResetPrimtive();

	static void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void PushAxis(const Vector3f& center, float length);

	static void PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color);

	static void PushSphere(const Vector3f& center, float radius, const Color4f& color);

	static DebugPrimitive* GetDebugPrimitive() { return debugPrimitive_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::unique_ptr<OffscreenTextureCollection> collection_;
	static inline std::unique_ptr<SkinningComputePipeline> skinningPipeline_;
	static inline std::unique_ptr<DebugPrimitive> debugPrimitive_;

};
