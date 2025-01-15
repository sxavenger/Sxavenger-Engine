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

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneRenderer {
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

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaqueGeometries(const DirectXThreadContext* context);

	void ProcessLighting(const DirectXThreadContext* context);

	void RenderTransparentGeometries(const DirectXThreadContext* context);

};
