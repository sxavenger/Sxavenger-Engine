#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FPriorityTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FPriorityTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool Compatible(const Option& other) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui resolution = {};

		float clearDepth     = 1.0f;
		uint8_t clearStencil = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPriorityTexture() = default;
	FPriorityTexture(const Vector2ui& resolution) { Create({ .resolution = resolution }); }

	void Create(const Option& option);

	//* transition option *//

	void ClearDepthStencil(const DirectXQueueContext* context);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const DxObject::Descriptor& GetDescriptorDSV() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;
	
	DxObject::Descriptor descriptorDSV_;

	//* option *//

	Option option_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Option& option);
	void CreateDescriptor();

};

SXAVENGER_ENGINE_NAMESPACE_END
