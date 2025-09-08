#include "AudioController.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AudioController::Init() {

	auto hr = XAudio2Create(&xaudio_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	Exception::Assert(SUCCEEDED(hr), "IXAudio2 create failed.");

	hr = xaudio_->CreateMasteringVoice(&master_, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);
	Exception::Assert(SUCCEEDED(hr), "IXAudio2MasteringVoice create failed.");

	for (auto tag : magic_enum::enum_values<AudioSourceComponent::Tag>()) {
		hr = xaudio_->CreateSubmixVoice(&submix_[static_cast<size_t>(tag)], kDefaultInputChannel, kDefaultSampleRate);
		Exception::Assert(SUCCEEDED(hr), "IXAudio2SubmixVoice create failed.");
	}

}

void AudioController::Term() {
	xaudio_.Reset();
}

void AudioController::SetVolume(AudioSourceComponent::Tag tag, float volume) {
	submix_[static_cast<size_t>(tag)]->SetVolume(volume);
}

float AudioController::GetVolume(AudioSourceComponent::Tag tag) const {
	float volume;
	submix_[static_cast<size_t>(tag)]->GetVolume(&volume);
	return volume;
}

IXAudio2SourceVoice* AudioController::CreateSource(AudioSourceComponent::Tag tag, const WAVEFORMATEX& format) {
	IXAudio2SourceVoice* source = nullptr;

	XAUDIO2_SEND_DESCRIPTOR descriptor = {};
	descriptor.pOutputVoice = GetSubmix(tag);

	XAUDIO2_VOICE_SENDS list = {};
	list.SendCount = 1;
	list.pSends    = &descriptor;

	auto hr = xaudio_->CreateSourceVoice(&source, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &list);
	Exception::Assert(SUCCEEDED(hr), "IXAudio2SourceVoice create failed.");

	return source;
}

AudioController* AudioController::GetInstance() {
	static AudioController instance;
	return &instance;
}
