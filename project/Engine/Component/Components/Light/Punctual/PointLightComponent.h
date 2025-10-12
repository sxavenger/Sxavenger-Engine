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
// PointLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class PointLightComponent
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
		LightCommon::Units unit;
		float intensity;
		float radius;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PointLightComponent(MonoBehaviour* behaviour);
	~PointLightComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	void SetUnit(LightCommon::Units unit);

	const Parameter& GetParameter() const;

	//* behaviour component option *//

	const TransformComponent* RequireTransform() const;

	//* serialization option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

};
