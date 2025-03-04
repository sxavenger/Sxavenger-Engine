#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AudioBuffer/AudioBuffer.h>

//* xaudio2
#include <xaudio2.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Audio class
////////////////////////////////////////////////////////////////////////////////////////////
class Audio {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Audio()  = default;
	~Audio() { Reset(); }

	void Create(IXAudio2* xaudio, const AudioBuffer* buffer, bool isLoop = false);

	void Reset();

	//* option *//

	void Play();

	void Stop();

	//* setter *//

	void SetVolume(float volume);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio *//

	IXAudio2SourceVoice* source_ = nullptr;

	//* parameter *//

	float volume_ = 1.0f;

};
