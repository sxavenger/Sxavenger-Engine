#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* audio
#include "AudioCommon.h"

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
// Audio3dSourceComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class Audio3dSourceComponent final
	: public BaseComponent {
	//!< FIXME: 3D Audio の聞こえる位置が正しくない問題を修正する.
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Channel enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Channel : UINT32 {
		Monoral = 1,
		Stereo  = 2,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	Audio3dSourceComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~Audio3dSourceComponent() override = default;

	void Play();

	void Update(const X3DAUDIO_LISTENER& listener);

	void SetVolume(float volume);

	void SetLoop(bool isLoop) { isLoop_ = isLoop; }

	void SetAudio(const AssetParameter<AssetAudioClip>& audio) { audio_ = audio; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	float volume_ = 1.0f;
	bool isLoop_ = false;

	Channel channel_ = Channel::Stereo;

	//* audio clip *//

	AssetParameter<AssetAudioClip> audio_ = nullptr;

	//* audio handle *//

	std::unique_ptr<AudioHandle> handle_ = nullptr;

	static inline std::array<float, 2> kDefaultStereoAzimuths = { kPi * 0.25f, kPi * 0.75f };
	
	//=========================================================================================
	// private methods
	//=========================================================================================

	//* behaviour methods *//

	const TransformComponent* RequireTransform() const;

	//* helper methods *//

	const X3DAUDIO_EMITTER GetEmitter() const;

};

SXAVENGER_ENGINE_NAMESPACE_END

