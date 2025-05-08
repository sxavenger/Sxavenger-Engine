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
#include <Engine/System/Runtime/Thread/AsyncTask.h>

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

		void Dispatch(const DirectXThreadContext* context);

		void Commit();

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* directX12 *//

		ComPtr<ID3D12Resource> asyncResource;
		DxObject::Descriptor asyncDescriptorUAV;

		ComPtr<ID3D12Resource> mainResource;
		DxObject::Descriptor mainDescriptorSRV;

		//* parameter *//

		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline; //!< HACK

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

		void Dispatch(const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* parameter *//

		static inline const UINT16 kMiplevels = 4;

		Vector2ui size;

		//* directX12 *//

		ComPtr<ID3D12Resource> asyncResource;
		std::array<DxObject::Descriptor, kMiplevels> asyncDescriptorUAVs;

		ComPtr<ID3D12Resource> mainResource;
		DxObject::Descriptor mainDescriptorSRV;

		//* dimension buffer *//

		std::unique_ptr<DxObject::DimensionBuffer<uint32_t>> indices;
		std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter;

		//* parameter *//

		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline; //!< HACK

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

	void Term();

	void Update();

	//* async option *//

	void Task(const DirectXThreadContext* context);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

	//* map *//

	IrradianceMap irradiance_;
	RadianceMap   radiance_;

	//* async task *//

	std::shared_ptr<AsyncTask> task_;

	//* parameter *//

	static inline constexpr UINT16 kCubemap_ = 6;
	static inline constexpr Vector2ui kNumThreads_ = { 16, 16 };

	//=========================================================================================
	// private variables
	//=========================================================================================

};

