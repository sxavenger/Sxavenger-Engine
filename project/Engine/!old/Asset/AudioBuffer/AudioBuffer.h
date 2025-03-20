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
#include <filesystem>

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

	AudioBuffer()  = default;
	~AudioBuffer() { Unload(); }

	void Load(const std::filesystem::path& filepath);

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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WAVEFORMATEX format_;     //!< 波形フォーマット
	BYTE*        pBuffer_;    //!< bufferの先頭アドレス
	uint32_t     bufferSize_; //!< バッファのサイズ

};
