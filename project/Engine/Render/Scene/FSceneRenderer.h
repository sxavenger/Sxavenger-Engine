#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetBuffer.h"
#include "../FRenderCore.h"
#include "FScene.h"

//* engine
//* component
#include <Engine/Component/Components/Camera/CameraComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneRenderer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsTechnique enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GraphicsTechnique : uint8_t {
		Deferred,
		Pathtracing,
		//Raytracing, //!< todo
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Status enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Status : uint32_t {
			Success = 0,

			//* vvv error type vvv *//
			Error_Buffer = 1 << 0,
			Error_Camera = 1 << 1,
			Error_Scene  = 1 << 2,
			Status_Error = Error_Buffer | Error_Camera | Error_Scene,

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Sxl::Flag<Status> CheckStatus() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* render target buffer *//

		FRenderTargetBuffer* buffer = nullptr;

		//* graphics setting *//

		GraphicsTechnique technique = GraphicsTechnique::Deferred;

		//* camera setting *//

		const CameraComponent* camera = nullptr;
		CameraComponent::Tag tag      = CameraComponent::Tag::GameCamera;

		//* scene setting *//

		FScene* scene = nullptr;

		//* lighting setting *//

		bool isEnableIndirectLighting = false;

		//* process setting *//

		bool isEnablePostProcess = true;
		bool isElableTonemap     = true;

		DxObject::SwapChain::ColorSpace colorSpace = DxObject::SwapChain::ColorSpace::Rec_709;
		
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Render(const DirectXQueueContext* context, const Config& config = {});

	//* resourviour

	void ResetReservoir();

	uint32_t GetReservoirSampleCount() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<FRenderCorePathtracing::Reservoir> reservoir_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* config helper methods *//

	Config ApplyConfig(const Config& config) const;

	//* base pass helper methods *//

	void RenderBasePass(const DirectXQueueContext* context, const Config& config);

	void RenderBasePassStaticMesh(const DirectXQueueContext* context, const Config& config);
	void RenderBasePassSkinnedMesh(const DirectXQueueContext* context, const Config& config);

	//* lighting pass helper methods *//

	void ProcessLightingPass(const DirectXQueueContext* context, const Config& config);

	void ProcessLightingPassEmpty(const DirectXQueueContext* context, const Config& config);
	void ProcessLightingPassDirectionalLight(const DirectXQueueContext* context, const Config& config);
	void ProcessLightingPassPointLight(const DirectXQueueContext* context, const Config& config);
	// todo: spot light
	// todo: rect light
	void ProcessLightingPassSkyLight(const DirectXQueueContext* context, const Config& config);

	void ProcessLightingPassIndirect(const DirectXQueueContext* context, const Config& config);
	void ProcessLightingPassIndirectDenoiser(const DirectXQueueContext* context, const Config& config);

	void LightingPassTransition(const DirectXQueueContext* context, const Config& config);

	//* ambient pass helper methods *//

	void ProcessAmbientPass(const DirectXQueueContext* context, const Config& config);

	void ProcessAmbientPassSkyBox(const DirectXQueueContext* context, const Config& config);

	//* transparent pass helper methods *//

	void RenderTransparentPass(const DirectXQueueContext* context, const Config& config);

	void RenderTransparentPassStaticMesh(const DirectXQueueContext* context, const Config& config);
	void RenderTransparentPassParticle(const DirectXQueueContext* context, const Config& config);

	//* post process helper methods *//

	void ProcessPostProcessPass(const DirectXQueueContext* context, const Config& config);

	//* tonemap pass helper methods *//

	void ProcessTonemapPass(const DirectXQueueContext* context, const Config& config);

};
