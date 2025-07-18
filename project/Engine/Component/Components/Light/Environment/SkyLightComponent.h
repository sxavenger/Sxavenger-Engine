#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../LightCommon.h"

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

		void SetTexture(uint32_t _index);

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

		void SetTexture(uint32_t _index, uint32_t _miplevels);

		void SetTexture(const AssetObserver<AssetTexture>& texture);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t index     = NULL;
		uint32_t miplevels = 0;

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

		//=========================================================================================
		// public varables
		//=========================================================================================

		float intensity = 1.0f;

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

	const D3D12_GPU_VIRTUAL_ADDRESS& GetParameterBufferAddress() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetDiffuseParameterBufferAddress() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetSpecularParameterBufferAddress() const;

	const Parameter& GetParameter() const;
	Parameter& GetParameter();

	const DiffuseParameter& GetDiffuseParameter() const;
	DiffuseParameter& GetDiffuseParameter();

	const SpecularParameter& GetSpecularParameter() const;
	SpecularParameter& GetSpecularParameter();

	void SetEnvironment(const D3D12_GPU_DESCRIPTOR_HANDLE& texture) { environment_ = texture; }
	void SetEnvironment(const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& texture) { environment_ = texture; }
	const std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>& GetEnvironment() const { return environment_; }

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	std::unique_ptr<DxObject::DimensionBuffer<DiffuseParameter>> diffuseParameter_;
	std::unique_ptr<DxObject::DimensionBuffer<SpecularParameter>> specularParameter_;
	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> environment_;

};
