#include "AssetTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/SxavengerContent.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Load(const DirectXQueueContext* context) {

	Texture::Option option = {}; //!< default option

	if (param_.has_value()) {
		option = std::any_cast<Texture::Option>(param_);
	}

	texture_.Load(context, BaseAsset::GetFilepath(), option);
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();

	if (!BaseAsset::IsComplete()) { //!< loadが完了していない場合
		ImGui::Text("loading...");
		return;
	}

	const D3D12_RESOURCE_DESC desc = texture_.GetResource()->GetDesc();

	if (ImGui::CollapsingHeader("Texture")) {
		if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
			ShowTexture();

		} else {
			ImGui::Text("texture dimension type is not D3D12_RESOURCE_DIMENSION_TEXTURE2D");
		}
		
	}

	if (ImGui::CollapsingHeader("Option")) {
		Texture::Option option = {};

		if (param_.has_value()) {
			option = std::any_cast<Texture::Option>(param_);
		}

		ImGui::Text("encoding: %s", GetEncoding(option.encoding).c_str());
	}

	if (ImGui::CollapsingHeader("Desc")) {
		ImGui::Text("dimension: %s", magic_enum::enum_name(desc.Dimension).data());
		ImGui::Text("width:     %u", desc.Width);
		ImGui::Text("height:    %u", desc.Height);
		ImGui::Text("depth:     %u", desc.DepthOrArraySize);
		ImGui::Text("miplevels: %u", desc.MipLevels);
		ImGui::Text("format:    %s", magic_enum::enum_name(desc.Format).data());
	}

	if (ImGui::CollapsingHeader("Descriptor")) {
		const DxObject::Descriptor& descriptor = texture_.GetDescriptorSRV();

		ImGui::Text("index:  %u",   descriptor.GetIndex());
		ImGui::Text("handle: 0x%p", descriptor.GetGPUHandle().ptr);
	}
}

const DxObject::Descriptor& AssetTexture::GetDescriptorSRV() const {
	if (!BaseAsset::IsComplete()) { //!< loadが完了していない場合
		return SxavengerContent::GetDescriptorSRV("checkerboard");
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

std::string AssetTexture::GetEncoding(Texture::Encoding encoding) {
	switch (encoding) {
		case Texture::Encoding::Lightness: return "Lightness (sRGB)";
		case Texture::Encoding::Intensity: return "Intensity";

		default: return "Unknown";
	}
}

void AssetTexture::ShowTexture() {

	Vector2ui texSize      = texture_.GetSize();
	Vector2f availableSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	// テクスチャの表示サイズを計算
	float scale = std::min(availableSize.x / texSize.x, availableSize.y / texSize.y);

	ImGui::Image(texture_.GetDescriptorSRV().GetGPUHandle().ptr, ImVec2(texSize.x * scale, texSize.y * scale));

}
