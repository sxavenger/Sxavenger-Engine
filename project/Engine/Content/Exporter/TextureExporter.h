#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <DirectXTex.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureExporter class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureExporter {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureDimension enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class TextureDimension : uint8_t {
		Texture1D,
		Texture2D,
		TextureCube,
		Texture3D,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Export(
		const DirectXQueueContext* context,
		TextureDimension dimension, ID3D12Resource* texture, DXGI_FORMAT format,
		const std::filesystem::path& filepath
	);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DirectX::ScratchImage GetImage(TextureDimension dimension, const D3D12_RESOURCE_DESC& desc);

	static DirectX::WICCodecs GetExtensionCodecs(const std::filesystem::path& extension);

	static void ExportTexture(const std::filesystem::path& filename, const DirectX::ScratchImage& image);

};
