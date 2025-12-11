#include "AssetAudioClip.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAudioClip class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAudioClip::Setup(IMFSourceReader* reader) {

	// PCM形式にフォーマットを指定する
	ComPtr<IMFMediaType> pcmType;
	MFCreateMediaType(&pcmType);
	pcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	auto hr = reader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pcmType.Get());

	// 実際にセットされたメディアタイプを取得する
	ComPtr<IMFMediaType> currentType;
	reader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &currentType);

	// 波形フォーマットを取得する
	WAVEFORMATEX* wave = nullptr;
	MFCreateWaveFormatExFromMFMediaType(currentType.Get(), &wave, nullptr);

	format_ = *wave;
	CoTaskMemFree(wave);

	// todo: メモリの予約

	while (true) {
		ComPtr<IMFSample> sample;
		DWORD index, flags;
		LONGLONG timestamp;

		// サンプルを取得する
		hr = reader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &index, &flags, &timestamp, &sample);

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) { 
			break; //!< 終端に到達したら終了
		}

		if (sample == nullptr) {
			continue;
		}

		ComPtr<IMFMediaBuffer> buffer;
		sample->ConvertToContiguousBuffer(&buffer);

		BYTE* ptr = nullptr;
		DWORD maxLength, currentLength;

		// バッファをロックしてデータを取得する
		buffer->Lock(&ptr, &maxLength, &currentLength);
		buffer_.insert(buffer_.end(), ptr, ptr + currentLength);
		buffer->Unlock();
	}

	BaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[AssetAudioClip]: audio clip setup complete. uuid: {}", BaseAsset::GetId().Serialize()));
}

void AssetAudioClip::ShowInspector() {
}

const WAVEFORMATEX& AssetAudioClip::GetFormat() const {
	BaseAsset::WaitComplete();
	return format_;
}

const BYTE* AssetAudioClip::GetBufferPointer() const {
	BaseAsset::WaitComplete();
	return buffer_.data();
}

const size_t AssetAudioClip::GetBufferSize() const {
	BaseAsset::WaitComplete();
	return buffer_.size();
}
