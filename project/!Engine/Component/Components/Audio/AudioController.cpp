#include "AudioController.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/DirectX/DirectXCommon.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AudioController::Init() {

	auto hr = XAudio2Create(&xaudio_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	DxObject::Assert(hr, L"IXAudio2 create failed.");

#ifdef _DEVELOPMENT
	XAUDIO2_DEBUG_CONFIGURATION config = {};
	config.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	config.BreakMask = XAUDIO2_LOG_ERRORS;
	xaudio_->SetDebugConfiguration(&config);
#endif

	hr = xaudio_->CreateMasteringVoice(&master_, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);
	DxObject::Assert(hr, L"IXAudio2MasteringVoice create failed.");

	for (auto tag : magic_enum::enum_values<AudioHandle::Tag>()) {
		hr = xaudio_->CreateSubmixVoice(&submix_[static_cast<size_t>(tag)], kDefaultInputChannel, kDefaultSampleRate);
		DxObject::Assert(hr, L"IXAudio2SubmixVoice create failed.");
	}

	X3DAudioInitialize(
		kDefaultInputChannel,
		X3DAUDIO_SPEED_OF_SOUND,
		handle_
	);

}

void AudioController::Term() {
	xaudio_.Reset();
}

void AudioController::SetVolume(AudioHandle::Tag tag, float volume) {
	submix_[static_cast<size_t>(tag)]->SetVolume(volume);
}

float AudioController::GetVolume(AudioHandle::Tag tag) const {
	float volume;
	submix_[static_cast<size_t>(tag)]->GetVolume(&volume);
	return volume;
}

XAUDIO2_VOICE_DETAILS AudioController::GetVoiceDetails(AudioHandle::Tag tag) const {
	XAUDIO2_VOICE_DETAILS details;
	submix_[static_cast<size_t>(tag)]->GetVoiceDetails(&details);
	return details;
}

IXAudio2SourceVoice* AudioController::CreateSource(AudioHandle::Tag tag, const WAVEFORMATEX& format) {
	IXAudio2SourceVoice* source = nullptr;

	XAUDIO2_SEND_DESCRIPTOR descriptor = {};
	descriptor.pOutputVoice = GetSubmix(tag);

	XAUDIO2_VOICE_SENDS list = {};
	list.SendCount = 1;
	list.pSends    = &descriptor;

	auto hr = xaudio_->CreateSourceVoice(&source, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &list);
	DxObject::Assert(hr, L"IXAudio2SourceVoice create failed.");

	return source;
}

void AudioController::CalculateAudio3d(const X3DAUDIO_LISTENER& listener, const X3DAUDIO_EMITTER& emitter, X3DAUDIO_DSP_SETTINGS& setting) const {
	X3DAudioCalculate(
		handle_,
		&listener,
		&emitter,
		X3DAUDIO_CALCULATE_MATRIX,
		&setting
	);
}

AudioController* AudioController::GetInstance() {
	static AudioController instance;
	return &instance;
}
