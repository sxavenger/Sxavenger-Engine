#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>

//* c++
#include <bitset>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewFlag enum
////////////////////////////////////////////////////////////////////////////////////////////
enum MultiViewFlag : uint8_t {
	VIEWFLAG_NONE = 0,
	VIEWFLAG_SRV = 1 << 0,
	VIEWFLAG_RTV = 1 << 1,
	VIEWFLAG_UAV = 1 << 2,

	//* options *//

	VIEWFLAG_RENDER_TEXTURE = VIEWFLAG_SRV | VIEWFLAG_RTV,
	VIEWFLAG_DUMMY_TEXTURE  = VIEWFLAG_SRV | VIEWFLAG_UAV,
};

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewTextureBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class MultiViewTextureBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MultiViewTextureBuffer() = default;
	~MultiViewTextureBuffer() { Term(); }

	void Create(
		uint8_t multiViewFlag, const Vector2ui& size, const Color4f& clearColor = {}, DXGI_FORMAT format = DxObject::kOffscreenFormat
	);

	void Term();

	//* transition methods *//

	void TransitionBeginRenderTarget(const DirectXThreadContext* context);
	void TransitionEndRenderTarget(const DirectXThreadContext* context);
	void ClearRenderTarget(const DirectXThreadContext* context);

	void TransitionBeginUnordered(const DirectXThreadContext* context);
	void TransitionEndUnordered(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	bool IsCreatedSRV() const;
	bool IsCreatedRTV() const;
	bool IsCreatedUAV() const;


	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const Vector2ui& GetSize() const { return size_; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ViewType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ViewType : uint8_t {
		kSRV,
		kRTV,
		kUAV
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	std::array<DxObject::Descriptor, ViewType::kUAV + 1> descriptors_;

	//* parameter *//

	std::bitset<ViewType::kUAV + 1> viewRecorder_;

	//* config *//

	Vector2ui   size_;
	Color4f     clearColor_;
	DXGI_FORMAT format_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource();
	void CreateSRV();
	void CreateRTV();
	void CreateUAV();

};
