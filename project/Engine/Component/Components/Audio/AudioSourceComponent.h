#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Tag : uint8_t {
		None, //!< 設定なし
		BGM,  //!< Background Music
		SE    //!< Sound Effect
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AudioHandle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AudioHandle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		AudioHandle(IXAudio2SourceVoice* source) : source(source) {}
		~AudioHandle();

		void Play();

		void Stop();

		void SetVolume(float volume);

		//=========================================================================================
		// public variables
		//=========================================================================================

		IXAudio2SourceVoice* const source = nullptr;

	};

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

	void SetTag(Tag tag) { tag_ = tag; }

	void SetVolume(float volume);

	void SetLoop(bool isLoop) { isLoop_ = isLoop; }

	void SetAudio(const AssetParameter<AssetAudioClip>& audio) { audio_ = audio; }


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	Tag tag_ = Tag::None;

	float volume_ = 1.0f;
	bool isLoop_  = false;

	//* audio clip *//

	AssetParameter<AssetAudioClip> audio_ = nullptr;

	//* handle *//

	std::unique_ptr<AudioHandle> handle_ = nullptr;

};
