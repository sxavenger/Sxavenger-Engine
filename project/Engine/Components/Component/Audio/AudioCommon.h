#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* xaudio2
#include <xaudio2.h>
#include <x3daudio.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AudioHandle structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 再生中の音声ボイスを識別・制御するハンドル
struct AudioHandle {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Tag : uint8_t {
		SE,  //!< Sound Effect
		BGM, //!< Background Music
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AudioHandle(IXAudio2SourceVoice* source) : source(source) {}
	~AudioHandle();

	void Play();

	void Stop();

	void SetVolume(float volume);

	void Flash();

	IXAudio2SourceVoice* GetSource() const { return source; }

	XAUDIO2_VOICE_DETAILS GetVoiceDetails() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	IXAudio2SourceVoice* const source = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
