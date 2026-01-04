#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../LightCommon.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Flag : uint32_t {
		None        = 0,
		Environment = 1 << 0, //!< Environment map
		Irradiance  = 1 << 1, //!< Irradiance map
		Radiance    = 1 << 2, //!< Radiance map
	};
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetEnvironment(uint32_t _index);

		void SetIrradiance(uint32_t _index);

		void SetRadiance(uint32_t _index, uint32_t _mipmaps = 0);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Sxl::Flag<Flag> flags;

		uint32_t environmentIndex;
		uint32_t irradianceIndex;
		uint32_t radianceIndex;

		float intensity;
		uint32_t radianceMipmaps;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyLightComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	~SkyLightComponent() override = default;

	void ShowComponentInspector() override;

	void Init();

	//* component option *//

	void SetEnvironment(const AssetParameter<AssetTexture>& texture);
	void SetEnvironment(const DxObject::Descriptor& texture);

	void SetIrradiance(const AssetParameter<AssetTexture>& texture);
	void SetIrradiance(const DxObject::Descriptor& texture);

	void SetRadiance(const AssetParameter<AssetTexture>& texture);
	void SetRadiance(const DxObject::Descriptor& texture, uint32_t mipmaps = 0);

	void SetIntensity(float intensity);

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	bool IsEnableEnvironment() const;

	bool IsEnableIrradiance() const;

	bool IsEnableRadiance() const;

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	//* serialize用
	AssetParameter<AssetTexture> environment_;
	AssetParameter<AssetTexture> irradiance_;
	AssetParameter<AssetTexture> radiance_;

};

SXAVENGER_ENGINE_NAMESPACE_END
