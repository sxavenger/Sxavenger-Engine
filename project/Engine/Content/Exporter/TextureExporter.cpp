#include "TextureExporter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* c++
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureExport class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureExporter::Export(
	const DirectXQueueContext* context,
	TextureDimension dimension, ID3D12Resource* texture, DXGI_FORMAT format,
	const std::filesystem::path& filepath) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// descの取得
	D3D12_RESOURCE_DESC textureDesc = texture->GetDesc();

	// imageの生成
	DirectX::ScratchImage image = GetImage(dimension, textureDesc.Format, textureDesc);
	const UINT kImageCount = static_cast<UINT>(image.GetImageCount());

	// image情報の取得
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints(kImageCount);
	UINT64 totalBytes = 0;
	device->GetCopyableFootprints(&textureDesc, 0, kImageCount, 0, footprints.data(), nullptr, nullptr, &totalBytes);
	

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

		Exception::Assert(SUCCEEDED(hr), "readback resource create failed.");
	}

	{ //!< readbackへcopy

		for (UINT i = 0; i < kImageCount; ++i) {

			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			dst.pResource       = readback.Get();
			dst.PlacedFootprint = footprints[i];

			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			src.pResource        = texture;
			src.SubresourceIndex = i;

			context->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		}

		context->ExecuteAllAllocators();
	}
	
	{ //!< GPUからImageに変更

		// mapping
		BYTE* map = nullptr;
		readback->Map(0, nullptr, reinterpret_cast<void**>(&map));

		std::span<const DirectX::Image> imgs = { image.GetImages(), kImageCount };

		for (size_t i = 0; i < kImageCount; ++i) {

			const DirectX::Image& img                           = imgs[i];
			const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint = footprints[i];

			const size_t kRowpitch = img.rowPitch;
			const size_t kHeight   = img.height;

			BYTE* dst = img.pixels;
			BYTE* src = map + footprint.Offset;

			for (size_t y = 0; y < kHeight; ++y) {
				std::memcpy(dst + y * kRowpitch, src + y * footprint.Footprint.RowPitch, kRowpitch);
			}
		}
	
		readback->Unmap(0, nullptr);
	}

	// imageのformat変換
	if (format != image.GetMetadata().format) {

		DirectX::ScratchImage converted = {};

		auto hr = DirectX::Convert(
			image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		Exception::Assert(SUCCEEDED(hr), "image convert failed.");

		image = std::move(converted);
	}

	ExportTexture(filepath, image);

	Logger::CommentRuntime("texture exported.", filepath.generic_string());
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
			hr = image.InitializeCube(format, size.x, size.y, size.z / 6, size.w);
			break;

		case TextureDimension::Texture3D:
			hr = image.Initialize3D(format, size.x, size.y, size.z, size.w);
			break;

		default: //!< exception
			Exception::Assert(false, "[TextureExporter] type is not define.");
			break;
	}

	Exception::Assert(SUCCEEDED(hr), "image create failed.");
	return image;
}

DirectX::WICCodecs TextureExporter::GetExtensionCodecs(const std::filesystem::path& extension) {
	if (extension == ".jpg") {
		return DirectX::WIC_CODEC_JPEG;

	} else if (extension == ".png") {
		return DirectX::WIC_CODEC_PNG;

	} else {
		Exception::Assert(false, "[TextureExporter] extension is not define.");
		return {};
	}
}

void TextureExporter::ExportTexture(const std::filesystem::path& filepath, const DirectX::ScratchImage& image) {

	const std::filesystem::path& extension = filepath.extension();

	HRESULT hr = {};

	if (extension == ".dds") {
		hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, filepath.generic_wstring().c_str());

	} else if (extension == ".tga") {
		hr = DirectX::SaveToTGAFile(*image.GetImages(), DirectX::TGA_FLAGS_NONE, filepath.generic_wstring().c_str());

	} else {
		hr = DirectX::SaveToWICFile(*image.GetImages(), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(GetExtensionCodecs(extension)), filepath.generic_wstring().c_str());
	}
}
