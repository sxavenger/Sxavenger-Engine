#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FEnvironmentMap class
////////////////////////////////////////////////////////////////////////////////////////////
class FEnvironmentMap {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// IrradianceMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct IrradianceMap {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector2ui& size);

		void Dispatch(const DirectXThreadContext* context, const D3D12_GPU_DESCRIPTOR_HANDLE& environment);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorSRV;
		DxObject::Descriptor descriptorUAV;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline;

		//* parameter *//

		Vector2ui size;

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateBuffer();

		void CreatePipeline();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RadianceMap structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RadianceMap {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Parameter structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct Parameter {
			Vector2ui size;
			uint32_t  miplevels;
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector2ui& _size);

		void Dispatch(const DirectXThreadContext* context, const D3D12_GPU_DESCRIPTOR_HANDLE& environment);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* parameter *//

		static inline const UINT16 kMiplevels = 10;

		Vector2ui size;

		//* directX12 *//

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorSRV;
		std::array<DxObject::Descriptor, kMiplevels> descriptorUAVs;

		//* dimension buffer *//

		std::unique_ptr<DxObject::DimensionBuffer<uint32_t>> indices;
		std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		void CreateBuffer();

		void CreateDimensionBuffer();

		void CreatePipeline();

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& size);

	void Dispatch(const DirectXThreadContext* context, const D3D12_GPU_DESCRIPTOR_HANDLE& environment);

	//* getter *//

	const IrradianceMap& GetIrradiance() const { return irradiance_; }

	const RadianceMap& GetRadiance() const { return radiance_; }

	const uint32_t GetIrradianceIndex() const { return irradiance_.descriptorSRV.GetIndex(); }

	const uint32_t GetRadianceIndex() const { return radiance_.descriptorSRV.GetIndex(); }

	static UINT16 GetRadianceMiplevel() { return RadianceMap::kMiplevels; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* map *//

	IrradianceMap irradiance_;
	RadianceMap   radiance_;

	//* parameter *//

	static inline constexpr UINT16 kCubemap_ = 6;
	static inline constexpr Vector2ui kNumThreads_ = { 16, 16 };

};
