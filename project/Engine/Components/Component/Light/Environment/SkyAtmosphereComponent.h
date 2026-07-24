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
#include <Engine/System/DirectX/DxObject/DxResource.h>
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
//! @brief 物理ベースの大気散乱(空/大気)をシミュレートするcomponent
class SkyAtmosphereComponent final
	: public BaseComponent {
public:
	// TODO: コンポーネント変数, 関数の整理

	PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Atmosphere structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 大気の物理パラメータ(散乱係数/惑星半径等)を保持するGPU構造体
	struct GPU_BUFFER_ALIGNAS Atmosphere {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Inspector();

		//=========================================================================================
		// public variables
		//=========================================================================================

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
		float mie_phase_param;

		Color3f ground_albedo;

		float top_radius;
		float bottom_radius;

		float multi_scattering_factor;

		float intensity;

	};

	// TODO: Light Parameter を作成

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Flag : uint32_t {
		None        = 0,
		Environment = 1 << 0,
		Aerial      = 1 << 1,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 大気散乱の描画設定(太陽方向/露出等)を保持するGPU構造体
	struct GPU_BUFFER_ALIGNAS Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetEnvironment(const DxObject::Descriptor& descriptorSRV);

		void SetAerial(const DxObject::Descriptor& descriptorSRV);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Sxl::Flag<Flag> flags;

		uint32_t environment;
		uint32_t aerial;

	};

	POP_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint8_t {
		Transmittance,
		MultipleScattering,
		SkyCube,
		Aerial
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Dynamic,   //!< リアルタイム計算
		Precomput, //!< 事前計算テクスチャを使用 (Transmittance, MultiScattering を事前計算したもととして扱う)
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyAtmosphereComponent(EntityBehaviour* behaviour);
	~SkyAtmosphereComponent() override = default;

	void ShowComponentInspector() override;

	void Update(Mode mode, const DirectXQueueContext* context);
	void Update(const DirectXQueueContext* context) { Update(mode_, context); }

	//* component option *//

	Mode GetMode() const { return mode_; }

	void SetMode(Mode mode) { mode_ = mode; }

	void SetIntensity(float intensity);

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	const TransformComponent* RequireTransform() const;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Texture structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 大気散乱のLUTテクスチャ(トランスミッタンス等)を保持する構造体
	struct Texture {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Dimension enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Dimension : uint8_t {
			Texture2D,
			TextureCube,
			Texture3D,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector3ui& resolution, DXGI_FORMAT format, Dimension dimension);

		//=========================================================================================
		// public variables
		//=========================================================================================

		DxObject::Resource resource;
		DxObject::Descriptor descriptorUAV;
		DxObject::Descriptor descriptorSRV;

		Vector3ui size;

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::unique_ptr<DxObject::ConstantBuffer<Atmosphere>> atmosphere_;
	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	//* mode *//

	Mode mode_ = Mode::Dynamic;

	//* precomputed textures *//

	std::array<Texture, magic_enum::enum_count<Type>()> textures_;

	//* pipeline state *//

	std::array<DxObject::ReflectionComputePipelineState, magic_enum::enum_count<Type>()> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper *//
	
	void CreateTransmittance();
	void CreateMultipleScattering();
	void CreateSkyCube();
	void CreateAerial();

	//* update helper *//

	void UpdateTransmittance(const DirectXQueueContext* context);
	void UpdateMultipleScattering(const DirectXQueueContext* context);
	void UpdateSkyCube(const DirectXQueueContext* context);
	void UpdateAerial(const DirectXQueueContext* context);

};

SXAVENGER_ENGINE_NAMESPACE_END
