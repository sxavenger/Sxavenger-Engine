#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FScene.h"
#include "FSceneTextures.h"

//* engine	
#include <Engine/System/DirectX/DirectXContext.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AViewActor;

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

	void Render(const DirectXThreadContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* scene *//

	FScene* scene_    = nullptr; //!< geometry and light actors

	//* view info *//

	AViewActor* view_ = nullptr; //!< camera actor
	std::unique_ptr<FSceneTextures> textures_;
	//! externalに変えるかも...

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RenderOpaque(const DirectXThreadContext* context);

	void RenderTransparent(const DirectXThreadContext* context);

};
