#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"
#include "FScene.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxVectorDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>
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
		Raytracing, //!< todo
		//Pathtracing,
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

	void Render(const DirectXThreadContext* context, const Config& config = {});

	//* setter *//

	void SetTextures(FRenderTargetTextures* textures) { textures_ = textures; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	FRenderTargetTextures* textures_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ApplyConfig(Config& config);

	Sxl::Flag<Status> CheckStatus(const Config& config) const;

	//* render *//

	void RenderGeometryPass(const DirectXThreadContext* context, const Config& config);
	void RenderGeometryStaticMeshDefault(const DirectXThreadContext* context, const Config& config);
	void RenderGeometrySkinnedMesh(const DirectXThreadContext* context, const Config& config);

	void LightingPass(const DirectXThreadContext* context, const Config& config);
	void LightingEmpty(const DirectXThreadContext* context, const Config& config);
	void LightingPassDirectionalLight(const DirectXThreadContext* context, const Config& config);
	void LightingPassPointLight(const DirectXThreadContext* context, const Config& config);
	void LightingPassRectLight(const DirectXThreadContext* context, const Config& config);
	void LightingPassSkyLight(const DirectXThreadContext* context, const Config& config);

	void AmbientProcessPass(const DirectXThreadContext* context, const Config& config);
	void AmbientProcessPassSkyBox(const DirectXThreadContext* context, const Config& config);

	void RenderTransparentBasePass(const DirectXThreadContext* context, const Config& config);
	void RenderTransparentBaseStaticMesh(const DirectXThreadContext* context, const Config& config);
	void RenderTransparentParticle(const DirectXThreadContext* context, const Config& config);

	void PostProcessPass(const DirectXThreadContext* context, const Config& config);

	void CompositeProcessPass(const DirectXThreadContext* context, const Config& config);
	void CompositeProcessPassTonemap(const DirectXThreadContext* context, const Config& config);

	//* technique *//

	void RenderTechniqueDeferred(const DirectXThreadContext* context, const Config& config);

	void RenderTechniqueRaytracing(const DirectXThreadContext* context, const Config& config);

};
