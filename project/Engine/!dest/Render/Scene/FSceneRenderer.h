#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FSceneTextures.h"
#include "FScene.h"
#include "Actor/Camera/ACameraActor.h"
#include "FAmbientProcessSetting.h"
#include "FPostProcessSetting.h"
#include "PostProcess/FPostProcessTextures.h"

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
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint32_t {
		Success        = 0,
		Error_Textures = 1 << 0,
		Error_Camera   = 1 << 1,

		Warning_Scene          = 1 << 2, //!< warning...?
		Warning_AmbientProcess = 1 << 3,
		Warning_PostProcess    = 1 << 4,

		//* vvv Error vvv *//

		Status_Error = Error_Textures | Error_Camera,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
		bool isEmptyLightAlbedo = false;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneRenderer()  = default;
	~FSceneRenderer() = default;

	//* render *//

	void Render(const DirectXThreadContext* context);

	Sxl::Flag<Status> CheckStatus() const;

	//* setter *//

	void SetTextures(FSceneTextures* textures) { textures_ = textures; }

	void SetCamera(ACameraActor* camera) { camera_ = camera; }

	void SetScene(FScene* scene) { scene_ = scene; }

	void SetAmbientSetting(FAmbientProcessSetting* setting) { ambientProcesses_ = setting; }

	void SetPostProcessSetting(FPostProcessSetting* setting) { postProcesses_ = setting; }

	//* getter *//

	Vector2ui GetSize() const { return textures_->GetSize(); }

	FScene* GetScene() const { return scene_; }

	const Config& GetConfig() const { return config_; }
	Config& GetConfig() { return config_; }

	ACameraActor* GetCamera() const { return camera_; }

	//* debug *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetDebugTexture() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	FSceneTextures* textures_ = nullptr;
	ACameraActor* camera_     = nullptr; //!< camera

	//* scene *//

	FScene* scene_ = nullptr; //!< geometry and light actors

	//* lighting parameter
	// todo: unlit parameter

	//* ambient process *//

	FAmbientProcessSetting* ambientProcesses_ = nullptr;

	//* post process *//

	FPostProcessSetting* postProcesses_ = nullptr;

	std::unique_ptr<FPostProcessTextures> processTextures_;
	static const size_t kProcessTextureSize = 3;

	//* raytracing *//

	DxrObject::StateObjectContext stateObjectContext_;

	//* config *//

	Config config_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaqueGeometries(const DirectXThreadContext* context);

	void ProcessLighting(const DirectXThreadContext* context);

	void ProcessAmbientPass(const DirectXThreadContext* context);

	void RenderTransparentGeometries(const DirectXThreadContext* context);

	/*void SetupRaytracing(const DirectXThreadContext* context);

	void ProcessRaytracingReflection(const DirectXThreadContext* context);*/

	void PostProcessPass(const DirectXThreadContext* context);

	//* sub method

	void RenderEmptyLight(const ALightActor::RendererContext& context);

	void RenderOpaqueGeometriesContainer(const AGeometryActor::Container& container, const AGeometryActor::RendererContext& context);

	void RenderTransparentGeometriesContainer(const AGeometryActor::Container& container, const AGeometryActor::RendererContext& context);

};
