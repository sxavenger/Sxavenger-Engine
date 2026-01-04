#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetAudioClip.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* xaudio2
#include <xaudio2.h>
#include <x3daudio.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Audio3dListenerComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class Audio3dListenerComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Audio3dListenerComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~Audio3dListenerComponent() override = default;

	const X3DAUDIO_LISTENER GetListener() const;

	uint32_t GetPriority() const { return priority_; }

	void SetPriority(uint32_t priority) { priority_ = priority; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	uint32_t priority_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	const TransformComponent* RequireTransform() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
