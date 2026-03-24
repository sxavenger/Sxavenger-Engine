#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FProbeTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FProbeTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProbeTexture()  = default;
	~FProbeTexture();

	void Create(const Vector3ui& probeCount, const Vector2ui& resolution, DXGI_FORMAT format);

	void SetName(const wchar_t* name) { resource_->SetName(name); }

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

	const Vector3ui& GetSize() const { return size_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorUAV_;

	Vector3ui size_ = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// FProbeGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FProbeGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Probe enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Probe : uint8_t {
		Sample,
		Reservoir,
		Moment,
		Irradiance,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void TransitionBeginUnordered(const DirectXQueueContext* context);

	void TransitionEndUnordered(const DirectXQueueContext* context);

	FProbeTexture* GetGBuffer(Probe probe) { return probe_[static_cast<size_t>(probe)].get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kProbeCount = magic_enum::enum_count<Probe>();

	static const std::array<DXGI_FORMAT, kProbeCount> kFormats;
	std::array<std::unique_ptr<FProbeTexture>, kProbeCount> probe_;

	//!< Deferred Probe 型に変更.
};

SXAVENGER_ENGINE_NAMESPACE_END
