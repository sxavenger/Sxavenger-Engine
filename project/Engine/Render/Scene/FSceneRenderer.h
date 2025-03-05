#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"
#include "Actor/Camera/ACameraActor.h" //!< todo: component化

//* engine
#include <Engine/System/DirectX/DxObject/DxVectorDimensionBuffer.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Component/ComponentStorage.h>

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

		//* vvv warning type vvv *//
		Warning_Scene          = 1 << 2,
		Warning_AmbientProcess = 1 << 3,
		Warning_PostProcess    = 1 << 4,

		//* vvv error vvv *//
		Status_Error = Error_Textures | Error_Camera,

		//* vvv warning vvv *//
		Status_Warning = Warning_Scene | Warning_AmbientProcess | Warning_PostProcess,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneRenderer()  = default;
	~FSceneRenderer() = default;

	void Render(const DirectXThreadContext* context);

	//* setter *//

	void SetTextures(FRenderTargetTextures* textures) { textures_ = textures; }

	void SetCamera(ACameraActor* camera) { camera_ = camera; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	FRenderTargetTextures* textures_ = nullptr;

	//* scene *//

	ACameraActor* camera_ = nullptr; //!< componentとして変換したい

	//* geometry pass parameter *//

	std::unique_ptr<DxObject::VectorDimensionBuffer<TransformationMatrix>>     transforms_;
	std::unique_ptr<DxObject::VectorDimensionBuffer<Material::MaterialBuffer>> materials_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Sxl::Flag<Status> CheckStatus() const;

	//* render *//

	void RenderGeometryPass(const DirectXThreadContext* context);

};
