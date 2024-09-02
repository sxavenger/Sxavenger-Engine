#include "Audio.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

//* c++
#include <algorithm>
#include <cwctype>

////////////////////////////////////////////////////////////////////////////////////////////
// AudioBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioBuffer::Load(const std::string& filename) {
	// .wavファイルを開く
	std::ifstream file;
	file.open(filename, std::ios_base::binary); //!< .wavをバイナリ形式で開く

	Assert(file.is_open()); //!< ファイルが開けなかった

	// riffヘッダーの読み込み
	RiffHeader riff = {};
	file.read((char*)&riff, sizeof(RiffHeader));

	// ファイルがriffか確認
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		Assert(false);
	}

	// タイプがwaveか確認
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		Assert(false);
	}

	FormatChunk format = {};

	// チャンクヘッダー確認
	file.read((char*)&format, sizeof(ChunkHeader));

	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		Assert(false);
	}

	// チャンク本体の書き込み
	Assert(format.chunk.size <= sizeof(WAVEFORMATEX));
	file.read((char*)&format.format, format.chunk.size);

	// dataチャンクの読み込み
	ChunkHeader data = {};
	file.read((char*)&data, sizeof(ChunkHeader));

	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);

		// 再度読み取り
		file.read((char*)&data, sizeof(ChunkHeader));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		Assert(false);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// waveファイルを閉じる
	file.close();

	format_     = format.format;
	pBuffer_    = reinterpret_cast<BYTE*>(pBuffer);
	bufferSize_ = data.size;

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

	auto manager = AudioManager::GetInstance();

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
}

void AudioManager::Term() {

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

AudioManager* AudioManager::GetInstance() {
	static AudioManager instance;
	return &instance;
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
	master_->SetVolume(kMasterVolume_);
}
