#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../../FSceneTextures.h"
#include "../FScene.h"
#include "../Actor/Camera/ACameraActor.h"
#include "../FPostProcessSetting.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseRenderer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RenderStatus enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RenderStatus : uint8_t {
		Succeed                     = 0,
		FSceneTextures_Error        = 1 << 0,
		FScene_Error                = 1 << 1,
		ACameraActor_Error          = 1 << 2,
		FPostProcessSetting_Warning = 1 << 3,

		//* errorは描画不可
		//* warningは警告
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseRenderer()          = default;
	virtual ~FBaseRenderer() = default;

	//* option *//

	Sxl::Flag<RenderStatus, uint8_t> CheckStatus() const;

	//* render *//

	virtual void Render(const DirectXThreadContext* context) = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* textures *//

	FSceneTextures* textures_ = nullptr;

	//* scene *//

	FScene* scene_        = nullptr; //!< geometry and light actors
	ACameraActor* camera_ = nullptr; //!< camera

	//* post process *//

	FPostProcessSetting* setting_ = nullptr;

	std::unique_ptr<FPostProcessTextures> processTextures_;
	static const size_t kProcessTextureSize = 2;

};
