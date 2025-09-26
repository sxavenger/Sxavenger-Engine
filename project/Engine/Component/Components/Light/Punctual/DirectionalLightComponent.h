#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../../Transform/TransformComponent.h"

//* light
#include "../LightCommon.h"

//* lib
#include <Lib/Geometry/Color3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectionalLightComponent final
	: public BaseComponent {
public:

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
		// public variables
		//=========================================================================================

		LightCommon::InlineShadow shadow;

		Color3f color;
		float   intensity; //!< [lux]

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectionalLightComponent(MonoBehaviour* behaviour);
	~DirectionalLightComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const Parameter& GetParameter() const;
	Parameter& GetParameter();

	//* behaviour component option *//

	const TransformComponent* RequireTransform() const;

	//* serialization option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
