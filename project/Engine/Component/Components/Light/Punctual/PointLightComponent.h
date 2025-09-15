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

	PointLightComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	virtual ~PointLightComponent() = default;

	virtual void ShowComponentInspector() override;

	void Init();

	//* component option *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

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

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
