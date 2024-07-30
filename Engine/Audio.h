#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// xaudio
#include <xaudio2.h>

// c++
#include <fstream>
#include <unordered_map>
#include <memory>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "xaudio2.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// Audio class
////////////////////////////////////////////////////////////////////////////////////////////
class Audio {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Audio() = default;

	~Audio() { SoundUnload(); }

	void SoundLoadWave(const std::string& filename);

	void SoundUnload();

	const WAVEFORMATEX& GetFormat() const { return format_; }

	const BYTE* const GetBuffer() const { return pBuffer_; }

	const uint32_t GetBufferSize() const { return bufferSize_; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// intermediate structure
	////////////////////////////////////////////////////////////////////////////////////////////

	struct ChunkHeader {
		char    id[4]; //!< チャンク毎のid
		int32_t size;  //!< チャンクサイズ
	};

	struct RiffHeader {
		ChunkHeader chunk;   //!< "RIFF"
		char        type[4]; //!< "WAVE"
	};

	struct FormatChunk {
		ChunkHeader  chunk;  //!< "fmt"
		WAVEFORMATEX format; //!< 波形フォーマット
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	WAVEFORMATEX format_;     //!< 波形フォーマット
	BYTE*        pBuffer_;    //!< bufferの先頭アドレス
	uint32_t     bufferSize_; //!< バッファのサイズ

};

////////////////////////////////////////////////////////////////////////////////////////////
// AudioManager class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理(シングルトンなので呼び出し必須)
	void Init();

	//! @brief 終了処理(シングルトンなので呼び出し必須)
	void Term();

	//! @brief Audioの読み込み
	const Audio* LoadAudio(const std::string& filename);

	void PlayAudio(const Audio* audio);
	void PlayAudio(const std::string& filename);

	//* singleton *//

	static AudioManager* GetInstance();

	AudioManager() = default;
	~AudioManager() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio *//

	ComPtr<IXAudio2>        xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	//* audio container *//

	std::unordered_map<std::string, std::unique_ptr<Audio>> audios_;

};
