#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxDescriptorHeaps.h>
#include <Engine/System/DirectXCommon.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewFlag enum
////////////////////////////////////////////////////////////////////////////////////////////
typedef uint32_t MultiViewFlag; //!< enum -> MultiViewFlag_
enum MultiViewFlag_ : uint32_t {
	VIEWFLAG_NONE = 0,
	VIEWFLAG_SRV = 1 << 0,
	VIEWFLAG_RTV = 1 << 1,
	VIEWFLAG_UAV = 1 << 2,

	//* options *//

	VIEWFLAG_RENDER_TEXTURE = VIEWFLAG_SRV | VIEWFLAG_RTV,
	VIEWFLAG_DUMMY_TEXTURE  = VIEWFLAG_SRV | VIEWFLAG_UAV,
};

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class MultiViewTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MultiViewTexture() = default;

	~MultiViewTexture() { Term(); }

	void Create(
		MultiViewFlag flag, const Vector2ui& size, const Color4f& clearColor = kDefaultClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat
	);

	void Term();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	//* handle

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	//* config

	const Vector2ui& GetSize() const { return size_; }

	const Color4f& GetClearColor() const { return clearColor_; }

	//* view checker

	bool IsCreateSRV() const { return viewRecorder_ & VIEWFLAG_SRV; }

	bool IsCreateRTV() const { return viewRecorder_ & VIEWFLAG_RTV; }

	bool IsCreateUAV() const { return viewRecorder_ & VIEWFLAG_UAV; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// MultiViewType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum MultiViewType : uint32_t {
		VIEWTYPE_SRV,
		VIEWTYPE_RTV,
		VIEWTYPE_UAV,

		kCountOfMultiViewType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DxObject::DescriptorHeaps* descriptorHeaps_ = nullptr;

	//* resource *//

	ComPtr<ID3D12Resource> resource_;

	//* view *//

	DxObject::Descriptor descriptors_[kCountOfMultiViewType];

	//* config *//

	MultiViewFlag viewRecorder_ = VIEWFLAG_NONE;
	Vector2ui size_;
	Color4f clearColor_;
	DXGI_FORMAT format_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateResource();
	void CreateSRV();
	void CreateRTV();
	void CreateUAV();

};