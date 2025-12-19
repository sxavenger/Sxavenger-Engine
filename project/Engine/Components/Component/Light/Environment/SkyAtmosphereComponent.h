#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../LightCommon.h"

//* component
#include "../../BaseComponent.h"
#include "../../Transform/TransformComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/DirectXAlignment.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyAtmosphereComponent final
	: public BaseComponent {
public:
	// TODO: コンポーネント変数, 関数の整理
	
	_PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Atmosphere structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct _GPU_BUFFER_ALIGNAS Atmosphere {
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

		float intensity;


		void Init();

		void Inspector();

	};

	// TODO: Light Parameter を作成

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Flag : uint32_t {
		None        = 0,
		Environment = 1 << 0,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct _GPU_BUFFER_ALIGNAS Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetEnvironment(const DxObject::Descriptor& descriptorSRV);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Sxl::Flag<Flag> flags;

		uint32_t environment;

	};

	_POP_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint8_t {
		Transmittance,
		MultipleScattering,
		SkyView,
		SkyCube,
		// TODO: SkyViewからSkyCubeへの変換 Aerial影
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyAtmosphereComponent(MonoBehaviour* behaviour);
	~SkyAtmosphereComponent() override = default;

	void ShowComponentInspector() override;

	void UpdateTransmittance(const DirectXQueueContext* context);
	void UpdateMultipleScattering(const DirectXQueueContext* context);
	void UpdateSkyView(const DirectXQueueContext* context);
	void UpdateSkyCube(const DirectXQueueContext* context);

	void SetIntensity(float intensity);

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//=========================================================================================
	// public methods
	//=========================================================================================

	const TransformComponent* RequireTransform() const;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Texture structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Texture {
	public:

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorUAV;
		DxObject::Descriptor descriptorSRV;

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::unique_ptr<DxObject::ConstantBuffer<Atmosphere>> atmosphere_;
	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	//* precomputed textures *//

	std::array<Texture, magic_enum::enum_count<Type>()> textures_;

	//* pipeline state *//

	DxObject::ReflectionComputePipelineState pipeline1_;
	DxObject::ReflectionComputePipelineState pipeline2_;
	DxObject::ReflectionComputePipelineState pipeline3_;
	DxObject::ReflectionComputePipelineState pipeline4_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateTransmittance();
	void CreateMultipleScattering();
	void CreateSkyView();
	void CreateSkyCube();

};

SXAVENGER_ENGINE_NAMESPACE_END
