#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* audio
#include "AudioCommon.h"

//* engine
#include <Engine/Preview/Asset/AssetAudioClip.h>
#include <Engine/Preview/Asset/AssetParameter.h>

//* xaudio2
#include <xaudio2.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioSourceComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioSourceComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AudioSourceComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~AudioSourceComponent() override { Stop(); }

	//* component option *//

	void Play();

	void Stop();

	void PlayOneShot(const std::optional<AssetParameter<AssetAudioClip>>& audio = std::nullopt);

	void SetTag(AudioHandle::Tag tag) { tag_ = tag; }

	void SetVolume(float volume);

	void SetLoop(bool isLoop) { isLoop_ = isLoop; }

	void SetAudio(const AssetParameter<AssetAudioClip>& audio) { audio_ = audio; }


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	AudioHandle::Tag tag_ = AudioHandle::Tag::SE;

	float volume_ = 1.0f;
	bool isLoop_  = false;

	//* audio clip *//

	AssetParameter<AssetAudioClip> audio_ = nullptr;

	//* handle *//

	std::unique_ptr<AudioHandle> handle_ = nullptr;


};
