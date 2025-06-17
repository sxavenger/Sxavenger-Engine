#include "TextureExporter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureExport class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureExporter::Export(
	const DirectXThreadContext* context,
	TextureDimension dimension, ID3D12Resource* texture, DXGI_FORMAT format,
	const std::filesystem::path& filepath) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	D3D12_RESOURCE_DESC textureDesc = texture->GetDesc();

	UINT64 totalBytes = 0;
	device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &totalBytes);
	

	// read back textureの生成
	ComPtr<ID3D12Resource> readback;

	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_READBACK;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width            = totalBytes;
		desc.Height           = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags            = D3D12_RESOURCE_FLAG_NONE;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&readback)
		);

		Assert(SUCCEEDED(hr), "readback resource create failed.");
	}

	size_t footprintRowpitch = {};

	{ //!< textureのcopy

		// dst
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = readback.Get();
		dst.Type      = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &dst.PlacedFootprint, nullptr, nullptr, nullptr);

		footprintRowpitch = dst.PlacedFootprint.Footprint.RowPitch;

		// src
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = texture;
		src.Type      = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		context->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		context->ExecuteAllAllocators();
	}

	// ScratchImageへ変換
	DirectX::ScratchImage image = GetImage(dimension, format, textureDesc);
	
	{ //!< GPUからImageに変更

		// mapping
		void* map = nullptr;
		readback->Map(0, nullptr, &map);

		const DirectX::Image* img = image.GetImages();
		BYTE* dst = img->pixels;
		BYTE* src = reinterpret_cast<BYTE*>(map);

		const size_t rowpitch = img->rowPitch;
		const size_t height   = img->height;

		for (size_t y = 0; y < height; ++y) {
			std::memcpy(dst + y * rowpitch, src + y * footprintRowpitch, rowpitch);
		}
	
		readback->Unmap(0, nullptr);
	}

	ExportTexture(filepath, image);

	CommentRuntime("texture exported.", filepath.generic_string());
}

DirectX::ScratchImage TextureExporter::GetImage(TextureDimension dimension, DXGI_FORMAT format, const D3D12_RESOURCE_DESC& desc) {

	HRESULT hr = {};
	DirectX::ScratchImage image = {};

	Vector4uz size = {};
	size.x = static_cast<size_t>(desc.Width);
	size.y = static_cast<size_t>(desc.Height);
	size.z = static_cast<size_t>(desc.DepthOrArraySize);
	size.w = static_cast<size_t>(desc.MipLevels);


	switch (dimension) {
		case TextureDimension::Texture1D:
			hr = image.Initialize1D(format, size.x, size.z, size.w);
			break;

		case TextureDimension::Texture2D:
			hr = image.Initialize2D(format, size.x, size.y, size.z, size.w);
			break;

		case TextureDimension::TextureCube:
			hr = image.InitializeCube(format, size.x, size.y, size.z % 6, size.w);
			break;

		case TextureDimension::Texture3D:
			hr = image.Initialize3D(format, size.x, size.y, size.z, size.w);
			break;

		default: //!< exception
			Assert(false, "[TextureExporter] type is not define.");
			break;
	}

	Assert(SUCCEEDED(hr), "image create failed.");
	return image;
}

DirectX::WICCodecs TextureExporter::GetExtensionCodecs(const std::filesystem::path& extension) {
	if (extension == ".jpg") {
		return DirectX::WIC_CODEC_JPEG;

	} else if (extension == ".png") {
		return DirectX::WIC_CODEC_PNG;

	} else {
		Assert(false, "[TextureExporter] extension is not define.");
		return {};
	}
}

void TextureExporter::ExportTexture(const std::filesystem::path& filepath, const DirectX::ScratchImage& image) {

	const std::filesystem::path& extension = filepath.extension();

	HRESULT hr = {};

	if (extension == ".dds") {
		hr = DirectX::SaveToDDSFile(*image.GetImages(), DirectX::DDS_FLAGS_NONE, filepath.generic_wstring().c_str());

	} else if (extension == ".tga") {
		hr = DirectX::SaveToTGAFile(*image.GetImages(), DirectX::TGA_FLAGS_NONE, filepath.generic_wstring().c_str());

	} else {
		hr = DirectX::SaveToWICFile(*image.GetImages(), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(GetExtensionCodecs(extension)), filepath.generic_wstring().c_str());
	}
}
