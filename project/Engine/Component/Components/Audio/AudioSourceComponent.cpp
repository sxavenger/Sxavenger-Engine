#include "AudioSourceComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* audio
#include "AudioController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AudioHandle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

AudioSourceComponent::AudioHandle::~AudioHandle() {
	Stop();
	source->DestroyVoice();
}

void AudioSourceComponent::AudioHandle::Play() {
	auto hr = source->Start();
	DxObject::Assert(hr, L"IXAudio2SourceVoice start failed.");
}

void AudioSourceComponent::AudioHandle::Stop() {
	source->Stop();
	source->FlushSourceBuffers();
}

void AudioSourceComponent::AudioHandle::SetVolume(float volume) {
	source->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////////////////
// AudioSourceComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioSourceComponent::Play() {

	auto clip = audio_.WaitRequire();

	IXAudio2SourceVoice* source = sAudioController->CreateSource(tag_, clip->GetFormat());

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = clip->GetBufferPointer();
	buf.AudioBytes = static_cast<UINT32>(clip->GetBufferSize());
	buf.Flags      = XAUDIO2_END_OF_STREAM;
	buf.LoopCount  = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

	auto hr = source->SubmitSourceBuffer(&buf);
	DxObject::Assert(hr, L"IXAudio2SourceVoice submit buffer failed.");

	handle_ = std::make_unique<AudioHandle>(source);
	handle_->SetVolume(volume_);
	handle_->Play();
}

void AudioSourceComponent::Stop() {
	if (handle_) {
		handle_->Stop();
		handle_.reset();
	}
}

void AudioSourceComponent::PlayOneShot(const std::optional<AssetParameter<AssetAudioClip>>& audio) {

	auto clip = audio.value_or(audio_).WaitRequire();

	IXAudio2SourceVoice* source = sAudioController->CreateSource(tag_, clip->GetFormat());

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = clip->GetBufferPointer();
	buf.AudioBytes = static_cast<UINT32>(clip->GetBufferSize());
	buf.Flags      = XAUDIO2_END_OF_STREAM;

	auto hr = source->SubmitSourceBuffer(&buf);
	DxObject::Assert(hr, L"IXAudio2SourceVoice submit buffer failed.");

	// 音量の設定
	source->SetVolume(volume_);

	hr = source->Start();
	DxObject::Assert(hr, L"IXAudio2SourceVoice start failed.");

}

void AudioSourceComponent::SetVolume(float volume) {
	volume_ = std::clamp(volume, 0.0f, 1.0f);

	if (handle_) {
		handle_->SetVolume(volume_);
	}
}
