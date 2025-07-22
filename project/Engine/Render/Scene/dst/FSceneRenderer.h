#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"
#include "../FRenderCore.h"
#include "FScene.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxVectorDimensionBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Component/Components/Light/Punctual/DirectionalLightComponent.h>
#include <Engine/Component/Components/Light/Punctual/PointLightComponent.h>
#include <Engine/Component/Components/Light/Rect/RectLightComponent.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/CompositeProcessLayer/CompositeProcessLayerComponent.h>

//* lib
#include <Lib/Sxl/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneRenderer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint32_t {
		Success = 0,

		//* vvv error type vvv *//
		Error_Textures = 1 << 0,
		Error_Camera   = 1 << 1,

		//* vvv error vvv *//
		Status_Error = Error_Textures | Error_Camera,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsTechnique enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GraphicsTechnique : uint8_t {
		Deferred,
		//Raytracing, //!< todo
		Pathtracing,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* graphics setting *//

		GraphicsTechnique technique = GraphicsTechnique::Deferred;

		//* camera setting *//

		const CameraComponent* camera = nullptr;
		CameraComponent::Tag tag      = CameraComponent::Tag::GameCamera;

		//* scene setting *//

		FScene* scene = nullptr;

		//* process setting *//

		bool isEnablePostProcess = true;

		//* composite setting *//

		bool isEnableComposite = true;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneRenderer()  = default;
	~FSceneRenderer() = default;

	void Render(const DirectXQueueContext* context, DirectXWindowContext* window, const Config& config = {});

	//* setter *//

	void SetTextures(FRenderTargetTextures* textures) { textures_ = textures; }

	//* option *//

	void ResetReserviour(const DirectXQueueContext* context);

	const uint32_t GetCurrentSampleCount() const;

	//* debug *//

	void DebugGui();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	FRenderTargetTextures* textures_ = nullptr;

	//* path tracing parameter *//

	std::unique_ptr<DxObject::DimensionBuffer<FRenderCorePathtracing::Reservoir>> reservoir_;
	bool isResetReservoir_ = false;

	std::unique_ptr<DxObject::DimensionBuffer<Vector3f>> test_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ApplyConfig(Config& config);

	Sxl::Flag<Status> CheckStatus(const Config& config) const;

	//* deferred render helper methods *//

	void RenderGeometryPass(const DirectXQueueContext* context, const Config& config);
	void RenderGeometryStaticMeshDefault(const DirectXQueueContext* context, const Config& config);
	void RenderGeometrySkinnedMesh(const DirectXQueueContext* context, const Config& config);

	void LightingPass(const DirectXQueueContext* context, const Config& config);
	void LightingEmpty(const DirectXQueueContext* context, const Config& config);
	void LightingPassDirectionalLight(const DirectXQueueContext* context, const Config& config);
	void LightingPassPointLight(const DirectXQueueContext* context, const Config& config);
	void LightingPassRectLight(const DirectXQueueContext* context, const Config& config);
	void LightingPassSkyLight(const DirectXQueueContext* context, const Config& config);

	void AmbientProcessPass(const DirectXQueueContext* context, const Config& config);
	void AmbientProcessPassSkyBox(const DirectXQueueContext* context, const Config& config);

	void RenderTransparentBasePass(const DirectXQueueContext* context, const Config& config);
	void RenderTransparentBaseStaticMesh(const DirectXQueueContext* context, const Config& config);
	void RenderTransparentParticle(const DirectXQueueContext* context, const Config& config);

	void PostProcessPass(const DirectXQueueContext* context, const Config& config);

	void CompositeProcessPass(const DirectXQueueContext* context, DirectXWindowContext* window, const Config& config);
	void CompositeProcessPassTonemap(const DirectXQueueContext* context, DirectXWindowContext* window, const Config& config);

	//* path tracing helper methods *//

	void RenderPathtracingPass(const DirectXQueueContext* context, const Config& config);

	void DenoiserPass(const DirectXQueueContext* context, const Config& config);

	//* technique *//

	void RenderTechniqueDeferred(const DirectXQueueContext* context, DirectXWindowContext* window, const Config& config);

	void RenderTechniqueRaytracing(const DirectXQueueContext* context, const Config& config);

	void RenderTechniquePathtracing(const DirectXQueueContext* context, const Config& config);


};
