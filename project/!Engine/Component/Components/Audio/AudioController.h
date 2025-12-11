#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* audio
#include "AudioCommon.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>

//* xaudio2
#include <xaudio2.h>
#include <x3daudio.h>

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
//! @brief Audio管理クラス
class AudioController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* controller option *//

	void SetVolume(AudioHandle::Tag tag, float volume);

	float GetVolume(AudioHandle::Tag tag) const;

	IXAudio2SubmixVoice* GetSubmix(AudioHandle::Tag tag) const { return submix_[static_cast<size_t>(tag)]; }

	XAUDIO2_VOICE_DETAILS GetVoiceDetails(AudioHandle::Tag tag) const;

	IXAudio2SourceVoice* CreateSource(AudioHandle::Tag tag, const WAVEFORMATEX& format);

	void CalculateAudio3d(const X3DAUDIO_LISTENER& listener, const X3DAUDIO_EMITTER& emitter, X3DAUDIO_DSP_SETTINGS& setting) const;
	
	//* singleton *//

	static AudioController* GetInstance();

	//=========================================================================================
	// public varaibles
	//=========================================================================================

	//* parameter *//

	static inline const UINT32 kDefaultInputChannel = 2;     //!< 入力チャンネル数
	static inline const UINT32 kDefaultSampleRate   = 44100; //!< サンプルレート

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio2 *//

	ComPtr<IXAudio2> xaudio_;

	IXAudio2MasteringVoice* master_                                                               = nullptr;
	std::array<IXAudio2SubmixVoice*, magic_enum::enum_count<AudioHandle::Tag>()> submix_ = {};

	X3DAUDIO_HANDLE handle_;
	
	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class instance
////////////////////////////////////////////////////////////////////////////////////////////
static AudioController* const sAudioController = AudioController::GetInstance();
