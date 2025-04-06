#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DiffuseParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DiffuseParameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetTexture(const AssetObserver<AssetTexture>& texture);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpecularParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpecularParameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetTexture(const AssetObserver<AssetTexture>& texture);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t index     = NULL;
		uint32_t miplevels = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkyLightComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	virtual ~SkyLightComponent() = default;

	virtual void ShowComponentInspector() override;

	void Init();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetDiffuseParameterBufferAddress() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetSpecularParameterBufferAddress() const;

	const DiffuseParameter& GetDiffuseParameter() const;
	DiffuseParameter& GetDiffuseParameter();

	const SpecularParameter& GetSpecularParameter() const;
	SpecularParameter& GetSpecularParameter();

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<DiffuseParameter>> diffuseParameter_;
	std::unique_ptr<DxObject::DimensionBuffer<SpecularParameter>> specularParameter_;

};
