#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/DxObject/DxResourceStateTracker.h>
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

		// todo: main resourceの使用.

		//* parameter option *//

		void SetEnvironment(const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& handle) { environment_ = handle; }

		const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& GetEnvironment() const { return environment_; }

		//* resource option *//

		const DxObject::Descriptor& UseDescriptorSRV(const DirectXThreadContext* context);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* directX12 *//

		DxObject::ResourceStateTracker asyncResource;
		DxObject::Descriptor asyncDescriptorUAV;

		DxObject::ResourceStateTracker mainResource;
		DxObject::Descriptor mainDescriptorSRV;

		//* parameter *//

		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline; //!< HACK

		//* parameter *//

		Vector2ui size;

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

		void Commit();

		//* parameter option *//

		void SetEnvironment(const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& handle) { environment_ = handle; }

		const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& GetEnvironment() const { return environment_; }

		//* resource option *//

		const DxObject::Descriptor& UseDescriptorSRV(const DirectXThreadContext* context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* parameter *//

		static inline const UINT16 kMiplevels = 4;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* parameter *//

		Vector2ui size;

		//* directX12 *//

		DxObject::ResourceStateTracker asyncResource;
		std::array<DxObject::Descriptor, kMiplevels> asyncDescriptorUAVs;

		DxObject::ResourceStateTracker mainResource;
		DxObject::Descriptor mainDescriptorSRV;

		//* dimension buffer *//

		std::unique_ptr<DxObject::DimensionBuffer<uint32_t>> indices;
		std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter;

		//* parameter *//

		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

		//* pipeline *//

		std::unique_ptr<DxObject::ReflectionComputePipelineState> pipeline; //!< HACK

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

	//* parameter option *//

	void SetEnvironment(const D3D12_GPU_DESCRIPTOR_HANDLE& environment) { environment_ = environment; }

	//* map option *//

	const DxObject::Descriptor& UseIrradianceDescriptor(const DirectXThreadContext* context);

	uint32_t GetRadianceMiplevels() const { return radiance_.kMiplevels; }

	const DxObject::Descriptor& UseRadianceDescriptor(const DirectXThreadContext* context);

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

