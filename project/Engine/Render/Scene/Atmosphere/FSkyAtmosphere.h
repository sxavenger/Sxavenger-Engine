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

	////////////////////////////////////////////////////////////////////////////////////////////
	// AtmosphereMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AtmosphereMap {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector2ui& size);

		void Dispatch(const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		ComPtr<ID3D12Resource> resource_; //!< cubemap.
		DxObject::Descriptor descriptorSRV_;
		DxObject::Descriptor descriptorUAV_;

		//* parameter *//

		Vector2ui size_;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline_;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateResource(const Vector2ui& size);

		void CreatePipeline();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// IrradianceMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct IrradianceMap {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const AtmosphereMap& atmosphere);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorSRV_;
		DxObject::Descriptor descriptorUAV_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RadianceMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RadianceMap {
	public:
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSkyAtmosphere()  = default;
	~FSkyAtmosphere() = default;

	void Create(const Vector2ui& size);

	void Update(const DirectXThreadContext* context);

	//* getter *//

	const AtmosphereMap& GetAtmosphere() { return atmosphere_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* map *//

	AtmosphereMap atmosphere_ = {};
	IrradianceMap irradiance_ = {};

	static inline const uint16_t kCubemap_ = 6;

};
