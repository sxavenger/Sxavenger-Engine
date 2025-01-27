#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* audio
#include "Audio.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>

//* xaudio2
#include <xaudio2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AudioController()  = default;
	~AudioController() = default;

	void Init();

	void Term();

	//* option *//

	std::unique_ptr<Audio> CreateAudio(const AudioBuffer* buffer, bool isLoop = false);

	void PlayOneShot(const AudioBuffer* buffer, float volume);

	//* getter *//

	IXAudio2* GetXAudio() const { return xaudio_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio *//

	ComPtr<IXAudio2> xaudio_        = nullptr;
	IXAudio2MasteringVoice* master_ = nullptr;

	//* parameter *//

	float volume_ = 0.8f;

};
