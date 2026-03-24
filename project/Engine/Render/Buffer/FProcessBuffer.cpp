#include "FProcessBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessBuffer::Create(uint8_t size, const FProcessTexture::Argument& argument) {
	
	for (auto& texture : textures_) {
		texture.Create(argument);
	}

	textures_.resize(size);
	for (uint8_t i = 0; i < textures_.size(); ++i) {
		textures_[i].Create(argument);

		std::wstring name = std::format(L"FProcessBuffer | [{}]", i);
		textures_[i].SetName(name);
	}
}

void FProcessBuffer::Import(const DirectXQueueContext* context, FRenderTexture* texture) {
	currentIndex_ = 0; //!< Importする際は常に最初のテクスチャから使用する.
	textures_.at(currentIndex_).ImportFromRenderTexture(context, texture);
}

void FProcessBuffer::Export(const DirectXQueueContext* context, FRenderTexture* texture) {
	textures_.at(currentIndex_).ExportToRenderTexture(context, texture);
}

void FProcessBuffer::Next(uint8_t count) {
	uint8_t next = (currentIndex_ + count) % textures_.size();
	currentIndex_ = next;
}

FProcessTexture& FProcessBuffer::GetCurrentTexture() {
	return textures_.at(currentIndex_);
}

FProcessTexture& FProcessBuffer::GetPreviousTexture(uint8_t previous) {
	StreamLogger::AssertA(previous <= textures_.size(), "process texture array size is not exist previous.");
	return textures_.at((currentIndex_ + textures_.size() - previous) % textures_.size());
}
