#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../LightCommon.h"

//* component
#include "../../BaseComponent.h"
#include "../../Transform/TransformComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/DirectXAlignment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyAtmosphereComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Texture structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Texture {
	public:

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorUAV;
		DxObject::Descriptor descriptorSRV;

	};
	
	_PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct _GPU_BUFFER_ALIGNAS Parameter {
	public:

		Vector3f rayleigh_scattering;
		float rayleigh_density_exp_scale;

		Vector3f absorption_extinction;
		float absorption_density_0_layer_height;
		float absorption_density_0_linear_term;
		float absorption_density_0_constant_term;
		float absorption_density_1_linear_term;
		float absorption_density_1_constant_term;

		Vector3f mie_scattering;
		float mie_density_exp_scale;
		Vector3f mie_extinction;

		float top_radius;
		float bottom_radius;

		Color3f ground_albedo;
		float multi_scattering_factor;


		void Init();

		void Inspector();

	};

	_POP_GPU_BUFFER_ALIGNAS

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyAtmosphereComponent(MonoBehaviour* behaviour);
	~SkyAtmosphereComponent() override = default;

	void ShowComponentInspector() override;

	void CreateTransmittance();
	void UpdateTransmittance(const DirectXQueueContext* context);

	void CreateMultipleScattering();
	void UpdateMultipleScattering(const DirectXQueueContext* context);

	void CreateSkyView();
	void UpdateSkyView(const DirectXQueueContext* context);

	void CreateSkyCube();
	void UpdateSkyCube(const DirectXQueueContext* context);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const {
		return skyCube_.descriptorSRV.GetGPUHandle();
	}

	//=========================================================================================
	// public methods
	//=========================================================================================

	const TransformComponent* RequireTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	//* precomputed textures *//

	Texture transmittance_;
	Texture multipleScattering_;
	Texture skyView_;
	Texture skyCube_;

	//* pipeline state *//

	DxObject::ReflectionComputePipelineState pipeline1_;
	DxObject::ReflectionComputePipelineState pipeline2_;
	DxObject::ReflectionComputePipelineState pipeline3_;
	DxObject::ReflectionComputePipelineState pipeline4_;

};
