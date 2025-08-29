#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* texture
#include "FBaseTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <array>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kDefaultOffscreenFormat);

	void Term();

	//* transition option *//

	D3D12_RESOURCE_BARRIER TransitionBeginUnordered() const;
	void TransitionBeginUnordered(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionEndUnordered() const;
	void TransitionEndUnordered(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionBeginState(D3D12_RESOURCE_STATES state) const;
	void TransitionBeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	D3D12_RESOURCE_BARRIER TransitionEndState(D3D12_RESOURCE_STATES state) const;
	void TransitionEndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	void BarrierUAV(const DirectXQueueContext* context) const;

	//* process option *//

	void GenerateMipmap(const DirectXQueueContext* context);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV(uint32_t mipLevel = 0) const;

	const Vector2ui& GetSize() const { return size_; }

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* parameter *//

	static inline const uint32_t kMipLevels                 = 6;
	static inline const D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	Vector2ui size_;

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor                         descriptorSRV_;
	std::array<DxObject::Descriptor, kMipLevels> descriptorsUAV_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTextureCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTextureCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessTextureCollection()  = default;
	~FProcessTextureCollection() = default;

	void Create(uint32_t count, const Vector2ui& size, DXGI_FORMAT format = DxObject::kDefaultOffscreenFormat);

	//* collection option *//

	void BeginProcess(const DirectXQueueContext* context, FBaseTexture* texture);

	void EndProcess(const DirectXQueueContext* context, FBaseTexture* texture);

	//* process option *//

	void NextProcess(uint32_t count = 1);

	void BarrierUAV(const DirectXQueueContext* context) const;

	FProcessTexture* GetPrevTexture(uint32_t prev = 1) const;

	FProcessTexture* GetCurrentTexture() const;

	void ReqiureCount(uint32_t count);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<std::unique_ptr<FProcessTexture>> textures_;

	uint32_t currentIndex_ = 0;
	//* 現在のresultとなるtextureのindex

};
