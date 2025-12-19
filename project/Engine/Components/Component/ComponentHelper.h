#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "Camera/CameraComponent.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHelper class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Component関連のHelperクラス
class ComponentHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* game loop helper *//

	static void BeginFrame();

	//* container helper *//

	static void UpdateTransform();

	static void UpdateSkinning();

	static void UpdateParticle();

	static void UpdateAudio3d();

	//* storage helper *//

	static void RegisterComponents();

	//* component helper *//

	static CameraComponent* GetCameraComponent(CameraComponent::Tag tag);

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
