#include "Audio.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Logger.h"
#include "Sxavenger.h"

//* c++
#include <algorithm>
#include <cwctype>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioBuffer::Load(const std::string& filename) {
	IMFSourceReader* pSourceReader = nullptr;
	auto hr = MFCreateSourceReaderFromURL(ToWstring(filename).c_str(), nullptr, &pSourceReader);
	Assert(SUCCEEDED(hr));

	IMFMediaType* pAudioType = nullptr;
	hr = MFCreateMediaType(&pAudioType);
	Assert(SUCCEEDED(hr));

	pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	hr = pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pAudioType);
	Assert(SUCCEEDED(hr));

	pAudioType->Release();

	IMFMediaType* pOutputType = nullptr;
	hr = pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
	Assert(SUCCEEDED(hr));

	UINT32 formatSize = 0;
	WAVEFORMATEX* pWaveFormat = nullptr;
	hr = MFCreateWaveFormatExFromMFMediaType(pOutputType, &pWaveFormat, &formatSize);
	Assert(SUCCEEDED(hr));

	// WAVEFORMATEXのコピー
	format_ = *pWaveFormat;

	// メモリを解放
	CoTaskMemFree(pWaveFormat);
	//pOutputType->Release();

	std::vector<BYTE> audioData;
	while (true) {
		IMFSample* sample = nullptr;
		DWORD dwStreamFlags = 0;
		pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &sample);


		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* buffer = nullptr;
		sample->ConvertToContiguousBuffer(&buffer);

		BYTE* buf = nullptr;
		DWORD currentLenght = 0;
		buffer->Lock(&buf, nullptr, &currentLenght);

		audioData.resize(audioData.size() + currentLenght);
		memcpy(audioData.data() + audioData.size() - currentLenght, buf, currentLenght);

		buffer->Unlock();
		buffer->Release();
		sample->Release();
	}

	pBuffer_ = new BYTE[audioData.size()];
	std::copy(audioData.begin(), audioData.end(), pBuffer_);
	bufferSize_ = static_cast<uint32_t>(audioData.size());

	pOutputType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, (UINT32*)&format_.nChannels);
	pOutputType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, (UINT32*)&format_.nSamplesPerSec);
	pOutputType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, (UINT32*)&format_.wBitsPerSample);

	format_.wFormatTag = WAVE_FORMAT_PCM;
	format_.nBlockAlign = format_.nChannels * format_.wBitsPerSample / 8;
	format_.nAvgBytesPerSec = format_.nSamplesPerSec * format_.nBlockAlign;

	pOutputType->Release();
	pSourceReader->Release();
}

void AudioBuffer::Unload() {

	delete[] pBuffer_;

	pBuffer_    = 0;
	bufferSize_ = 0;
	format_     = {};

}

////////////////////////////////////////////////////////////////////////////////////////////
// Audio class
////////////////////////////////////////////////////////////////////////////////////////////

void Audio::Create(const AudioBuffer* buffer, bool isLoop) {

	// 引数の保存
	buffer_ = buffer;
	isLoop_ = isLoop;

	auto manager = Sxavenger::GetAudioManager();

	// 波形formatを基にSourceVoiceの生成
	auto hr = manager->GetXAudio2()->CreateSourceVoice(
		&source_,
		&buffer_->GetFormat()
	);

	Assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = buffer_->GetBuffer();
	buf.AudioBytes = buffer_->GetBufferSize();
	buf.Flags      = XAUDIO2_END_OF_STREAM;
	buf.LoopCount  = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

	// 波形データの再生
	hr = source_->SubmitSourceBuffer(&buf);
	Assert(SUCCEEDED(hr));

}

void Audio::Term() {
	if (source_ != nullptr) {
		source_->Stop(); //!< 再生の停止
		source_->DestroyVoice();

		source_ = nullptr;
	}
}

void Audio::PlayAudio() {
	auto hr = source_->Start();
	Assert(SUCCEEDED(hr));
}

void Audio::StopAudio() {
	auto hr = source_->Stop();
	Assert(SUCCEEDED(hr));
}

void Audio::SetVolume(float volume) {
	volume_ = volume;
	source_->SetVolume(volume_);
}

void Audio::ResetAudio() {

	source_->Stop();

	source_->FlushSourceBuffers();

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = buffer_->GetBuffer();
	buf.AudioBytes = buffer_->GetBufferSize();
	buf.Flags      = XAUDIO2_END_OF_STREAM;
	buf.LoopCount   = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

	// 波形データの再生
	auto hr = source_->SubmitSourceBuffer(&buf);
	Assert(SUCCEEDED(hr));

	source_->SetVolume(volume_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// AudioManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

const std::string AudioManager::directory_ = "resources/sounds/";

void AudioManager::Init() {
	InitXAudio2();
	InitMfapi();
}

void AudioManager::Term() {
	MFShutdown();

	// xaudioの開放
	xAudio2_.Reset();

	// 音声データの解放
	buffers_.clear();
}

void AudioManager::LoadAudioBuffer(const std::string& filename) {
	std::string lower = ToLower(filename);

	auto it = buffers_.find(lower);
	if (it != buffers_.end()) {
		return; //!< すでにbufferが存在している
	}

	// bufferの生成
	std::unique_ptr<AudioBuffer> newBuffer = std::make_unique<AudioBuffer>();
	newBuffer->Load(directory_ + filename);

	// bufferの保存
	buffers_.emplace(lower, std::move(newBuffer));
}

const AudioBuffer* AudioManager::GetAudioBuffer(const std::string& filename) {

	std::string lower = ToLower(filename);

	auto it = buffers_.find(lower);
	if (it == buffers_.end()) { //!< bufferが存在しない場合
		LoadAudioBuffer(lower);
	}

	return buffers_.at(lower).get();

}

void AudioManager::PlayOneShot(const std::string& filename, float volume) {

	auto buffer = GetAudioBuffer(filename);

	IXAudio2SourceVoice* source = nullptr;

	// 波形formatを基にSourceVoiceの生成
	auto hr = xAudio2_->CreateSourceVoice(
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

std::unique_ptr<Audio> AudioManager::GetAudio(const std::string& filename, bool isLoop) {

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	audio->Create(GetAudioBuffer(filename), isLoop);
	
	return std::move(audio);
}

std::string AudioManager::ToLower(const std::string& str) {
	std::string result;
	result.resize(str.size());

	std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});

	return result;
}

void AudioManager::InitXAudio2() {

	auto hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	Assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&master_);
	Assert(SUCCEEDED(hr));

	// パラメーターの設定
	master_->SetVolume(masterVolume_);
}

void AudioManager::InitMfapi() {
	auto hr = MFStartup(MF_VERSION);
	Assert(SUCCEEDED(hr));
}
