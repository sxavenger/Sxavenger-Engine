#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FScene.h"
#include "FSceneTextures.h"
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

	void CreateTextures(const Vector2ui& size);

	//* render *//

	void Render(const DirectXThreadContext* context);

	//* setter *//

	void SetScene(FScene* scene) { scene_ = scene; }

	void SetCamera(ACameraActor* camera) { camera_ = camera; }

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

	//* view info *//

	ACameraActor* camera_ = nullptr; //!< camera
	std::unique_ptr<FSceneTextures> textures_;
	//! externalに変えるかも...

	//* raytracing *//

	DxrObject::StateObjectContext stateObjectContext_;

	//* config *//

	Config config_ = {};

	//* HACK

	DxObject::ReflectionComputePipelineState p_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaqueGeometries(const DirectXThreadContext* context);

	void ProcessLighting(const DirectXThreadContext* context);

	void RenderTransparentGeometries(const DirectXThreadContext* context);

	void SetupRaytracing(const DirectXThreadContext* context);

	void ProcessRaytracingReflection(const DirectXThreadContext* context);

};
