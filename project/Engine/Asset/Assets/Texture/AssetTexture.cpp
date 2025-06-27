#include "AssetTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/SxavengerContent.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Load(const DirectXThreadContext* context) {

	Texture::Encoding encoding = Texture::Encoding::Lightness; //!< default encoding

	if (param_.has_value()) {
		encoding = std::any_cast<Texture::Encoding>(param_);
	}

	texture_.Load(context, filepath_, encoding);
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();
}

const DxObject::Descriptor& AssetTexture::GetDescriptorSRV() const {
	if (!BaseAsset::IsComplete()) { //!< loadが完了していない場合
		return SxavengerContent::GetDescriptorSRV("white1x1");
		// todo: ロード中のtextureを変更
	}

	return texture_.GetDescriptorSRV();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& AssetTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

const Texture& AssetTexture::GetTexture() const {
	BaseAsset::WaitComplete();
	return texture_;
}
