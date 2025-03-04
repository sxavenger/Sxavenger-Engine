#include "Audio.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Audio class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Audio::Create(IXAudio2* xaudio, const AudioBuffer* buffer, bool isLoop) {
	Reset();

	// 波形formatを基にSourceVoiceの生成
	auto hr = xaudio->CreateSourceVoice(&source_, &buffer->GetFormat());
	Assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = buffer->GetBuffer();
	buf.AudioBytes = buffer->GetBufferSize();
	buf.Flags      = XAUDIO2_END_OF_STREAM;
	buf.LoopCount  = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	// 波形データの再生
	hr = source_->SubmitSourceBuffer(&buf);
	Assert(SUCCEEDED(hr));

	// 音量の設定
	source_->SetVolume(volume_);
}

void Audio::Reset() {
	if (source_ == nullptr) {
		return;
	}

	source_->Stop();
	source_->FlushSourceBuffers();
	source_->DestroyVoice();
	source_ = nullptr;
}

void Audio::Play() {
	auto hr = source_->Start();
	Assert(SUCCEEDED(hr));
}

void Audio::Stop() {
	auto hr = source_->Stop();
	Assert(SUCCEEDED(hr));
}

void Audio::SetVolume(float volume) {
	volume_ = volume;

	if (source_ != nullptr) {
		source_->SetVolume(volume_);
	}
}
