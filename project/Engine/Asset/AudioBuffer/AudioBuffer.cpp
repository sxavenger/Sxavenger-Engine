#include "AudioBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	struct Mfapi {
		Mfapi() {
			auto hr = MFStartup(MF_VERSION);
			Assert(SUCCEEDED(hr));
		}

		~Mfapi() {
			MFShutdown();
		}
	};

	Mfapi mfapi;
}

////////////////////////////////////////////////////////////////////////////////////////////
// AudioBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AudioBuffer::Load(const std::filesystem::path& filepath) {
	IMFSourceReader* pSourceReader = nullptr;
	auto hr = MFCreateSourceReaderFromURL(filepath.generic_wstring().c_str(), nullptr, &pSourceReader);
	Assert(SUCCEEDED(hr), "audio is not found. filepath: " + filepath.generic_string());

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

	format_.wFormatTag      = WAVE_FORMAT_PCM;
	format_.nBlockAlign     = format_.nChannels * format_.wBitsPerSample / 8;
	format_.nAvgBytesPerSec = format_.nSamplesPerSec * format_.nBlockAlign;

	pOutputType->Release();
	pSourceReader->Release();
}

void AudioBuffer::Unload() {
	if (pBuffer_) {
		delete[] pBuffer_;
		pBuffer_ = nullptr;
	}

	bufferSize_ = 0;
}
