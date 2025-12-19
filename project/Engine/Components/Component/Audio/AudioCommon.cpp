#include "AudioCommon.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioHandle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

AudioHandle::~AudioHandle() {
	Stop();
	source->DestroyVoice();
}

void AudioHandle::Play() {
	auto hr = source->Start();
	DxObject::Assert(hr, L"IXAudio2SourceVoice start failed.");
}

void AudioHandle::Stop() {
	source->Stop();
	Flash();
}

void AudioHandle::SetVolume(float volume) {
	source->SetVolume(volume);
}

void AudioHandle::Flash() {
	source->FlushSourceBuffers();
}

XAUDIO2_VOICE_DETAILS AudioHandle::GetVoiceDetails() const {
	XAUDIO2_VOICE_DETAILS details = {};
	source->GetVoiceDetails(&details);
	return details;
}
