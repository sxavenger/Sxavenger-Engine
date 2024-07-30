#include "Audio.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// Audio class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Audio::SoundLoadWave(const std::string& filename) {

	// .wavファイルを開く
	std::ifstream file;
	file.open(filename, std::ios_base::binary); //!< .wavをバイナリ形式で開く

	assert(file.is_open()); //!< ファイルが開けなかった

	// riffヘッダーの読み込み
	RiffHeader riff = {};
	file.read((char*)&riff, sizeof(RiffHeader));

	// ファイルがriffか確認
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(false);
	}

	// タイプがwaveか確認
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(false);
	}

	FormatChunk format = {};

	// チャンクヘッダー確認
	file.read((char*)&format, sizeof(ChunkHeader));

	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(false);
	}

	// チャンク本体の書き込み
	assert(format.chunk.size <= sizeof(WAVEFORMATEX));
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
		assert(false);
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

void Audio::SoundUnload() {

	delete[] pBuffer_;

	pBuffer_    = 0;
	bufferSize_ = 0;
	format_     = {};

}

////////////////////////////////////////////////////////////////////////////////////////////
// AudioManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioManager::Init() {

	auto hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

}

void AudioManager::Term() {

	// xaudioの開放
	xAudio2_.Reset();

	// 音声データの解放
	audios_.clear();
}

const Audio* AudioManager::LoadAudio(const std::string& filename) {
	
	// containerにすでにあるかに確認
	auto it = audios_.find(filename);
	if (it != audios_.end()) { //!< containerにaudioがある場合
		return it->second.get();
	}

	// ない場合は新しく作成
	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	audio->SoundLoadWave(filename);

	Audio* result = audio.get();

	audios_.emplace(filename, std::move(audio));

	return result;

}

void AudioManager::PlayAudio(const Audio* audio) {

	// 波形formatを基にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;

	auto hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &audio->GetFormat());
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = audio->GetBuffer();
	buf.AudioBytes = audio->GetBufferSize();
	buf.Flags      = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	hr = pSourceVoice->Start();
	assert(SUCCEEDED(hr));

}

void AudioManager::PlayAudio(const std::string& filename) {

	// containerにAudioが登録されているかの確認
	auto it = audios_.find(filename);
	if (it == audios_.end()) { //!< audioが見つからなかった
		assert(false);
		return;
	}

	PlayAudio(it->second.get());
}



AudioManager* AudioManager::GetInstance() {
	static AudioManager instance;
	return &instance;
}
