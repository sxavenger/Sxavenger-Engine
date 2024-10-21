#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* xaudio2
#include <xaudio2.h>

//* mfapi
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mfobjects.h>

//* c++
#include <fstream>
#include <unordered_map>
#include <memory>
#include <string>

//* ComPtr
#include "ComPtr.h"


//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
//* xaudio
#pragma comment(lib, "xaudio2.lib")

//* mfapi
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// AudioBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AudioBuffer() = default;

	~AudioBuffer() { Unload(); }

	void Load(const std::string& filename);

	void Unload();

	//* Getter *//

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
// Audio class
////////////////////////////////////////////////////////////////////////////////////////////
class Audio {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Audio() = default;

	~Audio() { Term(); }

	void Create(const AudioBuffer* buffer, bool isLoop = false);

	void Term();

	//* source option *//

	void PlayAudio();

	void StopAudio();

	void SetVolume(float volume);

	void ResetAudio();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio *//

	IXAudio2SourceVoice* source_ = nullptr;

	//* parameter *//

	const AudioBuffer* buffer_ = nullptr;
	bool               isLoop_ = false;
	float              volume_ = 1.0f;


};

////////////////////////////////////////////////////////////////////////////////////////////
// AudioManager class
////////////////////////////////////////////////////////////////////////////////////////////
class AudioManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* buffers *//

	void LoadAudioBuffer(const std::string& filename);

	const AudioBuffer* GetAudioBuffer(const std::string& filename);

	//* audio option *//

	void PlayOneShot(const std::string& filename, float volume);

	std::unique_ptr<Audio> GetAudio(const std::string& filename, bool isLoop);

	//* Getter *//

	IXAudio2* GetXAudio2() const { return xAudio2_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xaudio *//

	ComPtr<IXAudio2>        xAudio2_;
	IXAudio2MasteringVoice* master_ = nullptr;

	//* container *//

	std::unordered_map<std::string, std::unique_ptr<AudioBuffer>> buffers_;

	//* parameter *//

	float masterVolume_ = 0.04f;

	static const std::string directory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::string ToLower(const std::string& str);

	void InitXAudio2();
	void InitMfapi();



};