#include "AudioController.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioController::Init() {

	auto hr = XAudio2Create(&xaudio_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	Assert(SUCCEEDED(hr));

	hr = xaudio_->CreateMasteringVoice(&master_);
	Assert(SUCCEEDED(hr));

	// パラメーターの設定
	master_->SetVolume(volume_);

}

void AudioController::Term() {
	xaudio_.Reset();
}

std::unique_ptr<Audio> AudioController::CreateAudio(const AudioBuffer* buffer, bool isLoop) {
	auto audio = std::make_unique<Audio>();
	audio->Create(xaudio_.Get(), buffer, isLoop);
	return audio;
}

void AudioController::PlayOneShot(const AudioBuffer* buffer, float volume) {
	IXAudio2SourceVoice* source = nullptr;

	// 波形formatを基にSourceVoiceの生成
	auto hr = xaudio_->CreateSourceVoice(
		&source,
		&buffer->GetFormat()
	);
	Assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = buffer->GetBuffer();
	buf.AudioBytes = buffer->GetBufferSize();
	buf.Flags      = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	hr = source->SubmitSourceBuffer(&buf);
	Assert(SUCCEEDED(hr));

	source->SetVolume(volume);

	hr = source->Start();
	Assert(SUCCEEDED(hr));
}
