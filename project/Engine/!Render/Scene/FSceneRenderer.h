#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FSceneTextures.h"
#include "FScene.h"
#include "Actor/Camera/ACameraActor.h"

//* engine	
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>

#include "Engine/System/DirectX/DxObject/DxComputePipelineState.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneRenderer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
		bool isUseRaytracing = true;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneRenderer()  = default;
	~FSceneRenderer() = default;

	//* render *//

	void Render(const DirectXThreadContext* context);

	bool CheckRender() const;

	//* setter *//

	void SetScene(FScene* scene) { scene_ = scene; }

	void SetCamera(ACameraActor* camera) { camera_ = camera; }

	void SetTextures(FSceneTextures* textures) { textures_ = textures; }

	//* getter *//

	Vector2ui GetSize() const { return textures_->GetSize(); }

	FScene* GetScene() const { return scene_; }

	const Config& GetConfig() const { return config_; }
	Config& GetConfig() { return config_; }

	//* debug *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetDebugTexture() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* scene *//

	FScene* scene_ = nullptr; //!< geometry and light actors

	ACameraActor* camera_ = nullptr; //!< camera

	//* view info *//

	FSceneTextures* textures_ = nullptr;

	//* raytracing *//

	DxrObject::StateObjectContext stateObjectContext_;

	//* config *//

	Config config_ = {};

	//* HACK
	std::unique_ptr<DxObject::ReflectionComputePipelineState> p_;
	void HACKProcessSSAO(const DirectXThreadContext* context);

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaqueGeometries(const DirectXThreadContext* context);

	void ProcessLighting(const DirectXThreadContext* context);

	void RenderTransparentGeometries(const DirectXThreadContext* context);

	void SetupRaytracing(const DirectXThreadContext* context);

	void ProcessRaytracingReflection(const DirectXThreadContext* context);

	//* sub method

	void RenderEmptyLight(const ALightActor::RendererContext& context);

};
