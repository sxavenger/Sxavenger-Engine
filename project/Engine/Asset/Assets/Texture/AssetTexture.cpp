#include "AssetTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/SxavengerContent.h>

//* external
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Load(const DirectXThreadContext* context) {

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
		ImGui::Text("dimension: %s", GetDimension(desc.Dimension).c_str());
		ImGui::Text("width:     %u", desc.Width);
		ImGui::Text("height:    %u", desc.Height);
		ImGui::Text("depth:     %u", desc.DepthOrArraySize);
		ImGui::Text("miplevels: %u", desc.MipLevels);
		ImGui::Text("format:    %s", GetFormat(desc.Format).c_str());
	}

	if (ImGui::CollapsingHeader("Descriptor")) {
		const DxObject::Descriptor& descriptor = texture_.GetDescriptorSRV();

		ImGui::Text("index:  %u",  descriptor.GetIndex());
		ImGui::Text("handle: %p", descriptor.GetGPUHandle().ptr);
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

std::string AssetTexture::GetDimension(D3D12_RESOURCE_DIMENSION dimension) {
	switch (dimension) {
		case D3D12_RESOURCE_DIMENSION_UNKNOWN:   return "Unknown";
		case D3D12_RESOURCE_DIMENSION_BUFFER:    return "Buffer";
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return "Texture1D";
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return "Texture2D";
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return "Texture3D";

		default: return "Unsupported Dimension";
	}
}

std::string AssetTexture::GetFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_UNKNOWN: return "UNKNOWN";

		// typeless
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return "R32G32B32A32_TYPELESS";
		case DXGI_FORMAT_R32G32B32_TYPELESS:    return "R32G32B32_TYPELESS";

		// float
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return "R32G32B32A32_FLOAT";
		case DXGI_FORMAT_R32G32B32_FLOAT:    return "R32G32B32_FLOAT";
		case DXGI_FORMAT_R16G16B16A16_FLOAT: return "R16G16B16A16_FLOAT";
		case DXGI_FORMAT_R32G32_FLOAT:       return "R32G32_FLOAT";
		case DXGI_FORMAT_R11G11B10_FLOAT:    return "R11G11B10_FLOAT";
		case DXGI_FORMAT_R32_FLOAT:          return "R32_FLOAT";
		case DXGI_FORMAT_R16_FLOAT:          return "R16_FLOAT";

		// uint
		case DXGI_FORMAT_R32_UINT: return "R32_UINT";
		case DXGI_FORMAT_R16_UINT: return "R16_UINT";
		case DXGI_FORMAT_R8_UINT:  return "R8_UINT";

		// sint
		case DXGI_FORMAT_R32_SINT: return "R32_SINT";
		case DXGI_FORMAT_R16_SINT: return "R16_SINT";
		case DXGI_FORMAT_R8_SINT:  return "R8_SINT";

		// unorm
		case DXGI_FORMAT_R8G8B8A8_UNORM:        return "R8G8B8A8_UNORM";
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return "R8G8B8A8_UNORM_SRGB";
		case DXGI_FORMAT_B8G8R8A8_UNORM:        return "B8G8R8A8_UNORM";
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return "B8G8R8A8_UNORM_SRGB";
		case DXGI_FORMAT_R16G16_UNORM:          return "R16G16_UNORM";
		case DXGI_FORMAT_R16_UNORM:             return "R16_UNORM";

		// snorm
		case DXGI_FORMAT_R8G8B8A8_SNORM: return "R8G8B8A8_SNORM";
		case DXGI_FORMAT_R8_SNORM:       return "R8_SNORM";

		// depth-stencil
		case DXGI_FORMAT_D32_FLOAT:            return "D32_FLOAT";
		case DXGI_FORMAT_D24_UNORM_S8_UINT:    return "D24_UNORM_S8_UINT";
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return "D32_FLOAT_S8X24_UINT";

		// compressed
		case DXGI_FORMAT_BC1_UNORM:       return "BC1_UNORM";
		case DXGI_FORMAT_BC1_UNORM_SRGB:  return "BC1_UNORM_SRGB";
		case DXGI_FORMAT_BC2_UNORM:       return "BC2_UNORM";
		case DXGI_FORMAT_BC2_UNORM_SRGB:  return "BC2_UNORM_SRGB";
		case DXGI_FORMAT_BC3_UNORM:       return "BC3_UNORM";
		case DXGI_FORMAT_BC3_UNORM_SRGB:  return "BC3_UNORM_SRGB";
		case DXGI_FORMAT_BC4_UNORM:       return "BC4_UNORM";
		case DXGI_FORMAT_BC5_UNORM:       return "BC5_UNORM";
		case DXGI_FORMAT_BC6H_UF16:       return "BC6H_UF16";
		case DXGI_FORMAT_BC6H_SF16:       return "BC6H_SF16";
		case DXGI_FORMAT_BC7_UNORM:       return "BC7_UNORM";
		case DXGI_FORMAT_BC7_UNORM_SRGB:  return "BC7_UNORM_SRGB";

		// その他
		case DXGI_FORMAT_R10G10B10A2_UNORM:          return "R10G10B10A2_UNORM";
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return "R10G10B10_XR_BIAS_A2_UNORM";

		default: return "Unsupported Format";
	}
}

void AssetTexture::ShowTexture() {

	Vector2ui texSize      = texture_.GetSize();
	Vector2f availableSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	// テクスチャの表示サイズを計算
	float scale = std::min(availableSize.x / texSize.x, availableSize.y / texSize.y);

	ImGui::Image(texture_.GetDescriptorSRV().GetGPUHandle().ptr, ImVec2(texSize.x * scale, texSize.y * scale));

}
