#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DirectX12
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>

//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const Color4f kDefaultTextureClearColor = ToColor4f(0x3A504BFF);

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTexture base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTexture { //!< このクラス単体では使わない
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseTexture()          = default;
	virtual ~BaseTexture() { Term(); }

	void Term();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

	const DXGI_FORMAT GetFormat() const { return format_; }

protected:

	//=========================================================================================
	// prteocted variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorSRV_;

	//* paraemter *//

	Vector2ui size_;
	DXGI_FORMAT format_;


};

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture
	: public BaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Texture()  = default;
	~Texture() { Term(); }

	void Load(const std::string& filepath, DirectXThreadContext* context);

	void Term();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> intermediateResouce_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DirectX::ScratchImage LoadTexture(const std::string& filepath);

	static ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	static _NODISCARD ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	);
};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture
	: public BaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderTexture()  = default;
	~RenderTexture() { Term(); }

	void Create(const Vector2ui& size, const Color4f& clearColor = kDefaultTextureClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* render option *//

	void TransitionBeginRender(DirectXThreadContext* context);

	void TransitionEndRender(DirectXThreadContext* context);

	void ClearRender(DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }

	const Color4f& GetClearColor() const { return clearColor_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Color4f clearColor_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class UnorderedTexture
	: public BaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UnorderedTexture() = default;
	~UnorderedTexture() { Term(); }

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* unordered option *//

	void TransitionBeginUnordered(DirectXThreadContext* context);

	void TransitionEndUnordered(DirectXThreadContext* context);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorUAV_;

};

