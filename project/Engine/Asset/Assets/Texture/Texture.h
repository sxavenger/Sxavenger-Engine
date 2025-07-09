#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* DirectX12
#include <d3dx12.h>
#include <DirectXTex.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Encoding enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Encoding : bool {
		Lightness, //!< sRGB
		Intensity,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Encoding encoding     = Encoding::Lightness;
		bool isGenerateMipmap = true;

	};
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Metadata structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Metadata {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Assign(const DirectX::TexMetadata& metadata);
		// todo: resource自体の情報を持たせる.

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui size;
		uint32_t miplevels;
		DXGI_FORMAT format;
		bool isCubemap;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Texture() = default;
	~Texture() { Term(); }

	void Load(const DirectXThreadContext* context, const std::filesystem::path& filepath, const Option& option);

	void Term();

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const DxObject::Descriptor& GetDescriptorSRV() const { return descriptorSRV_; }

	const Metadata& GetMetadata() const { return metadata_; }

	const Vector2ui& GetSize() const { return metadata_.size; }

	const DXGI_FORMAT GetFormat() const { return metadata_.format; }

	ID3D12Resource* GetResource() const { return resource_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *// 

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	//* parameter *//

	Metadata metadata_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* encoding helper methods *//

	static Encoding GetFormatEncoding(DXGI_FORMAT format);

	//* texture load helper methods *//

	static DirectX::ScratchImage LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromWICFile(const std::filesystem::path& filepath, const Option& option);

	static DirectX::ScratchImage LoadTexture(const std::filesystem::path& filepath, const Option& option);

	static ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	static _NODISCARD ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
};
