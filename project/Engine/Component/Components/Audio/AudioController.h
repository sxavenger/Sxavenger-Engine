#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* audio
#include "AudioSourceComponent.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>

//* xaudio2
#include <xaudio2.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <cstdint>
#include <array>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
//* xaudio
#pragma comment(lib, "xaudio2.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* controller option *//

	void SetVolume(AudioSourceComponent::Tag tag, float volume);

	float GetVolume(AudioSourceComponent::Tag tag) const;

	IXAudio2SubmixVoice* GetSubmix(AudioSourceComponent::Tag tag) const { return submix_[static_cast<size_t>(tag)]; }

	IXAudio2SourceVoice* CreateSource(AudioSourceComponent::Tag tag, const WAVEFORMATEX& format);
	
	//* singleton *//

	static AudioController* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio2 *//

	ComPtr<IXAudio2> xaudio_;

	IXAudio2MasteringVoice* master_                                                               = nullptr;
	std::array<IXAudio2SubmixVoice*, magic_enum::enum_count<AudioSourceComponent::Tag>()> submix_ = {};

	//* parameter *//

	static inline const UINT32 kDefaultInputChannel = 2;     //!< 入力チャンネル数
	static inline const UINT32 kDefaultSampleRate   = 44100; //!< サンプルレート
	
	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class instance
////////////////////////////////////////////////////////////////////////////////////////////
static AudioController* const sAudioController = AudioController::GetInstance();
