#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class
////////////////////////////////////////////////////////////////////////////////////////////
class FSkyAtmosphere {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSkyAtmosphere()  = default;
	~FSkyAtmosphere() = default;

	void Create(const Vector2ui& size);

	void Dispatch(const DirectXThreadContext* context);

	const DxObject::Descriptor& GetDescriptorSRV() const { return cubemapSRV_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directX12 *//

	ComPtr<ID3D12Resource> cubemap_;
	DxObject::Descriptor cubemapSRV_;
	DxObject::Descriptor cubemapUAV_;

	//* pipeline *//

	std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline_;

	//* parameter *//

	Vector2ui size_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

};
