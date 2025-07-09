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
	: public BaseComponent, public LightCommon {
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

		Color3f color;
		Units unit;
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

	//* buffer option *//

	void CreateParameterBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetParameterBufferAddress() const;

	const Parameter& GetParameter() const;

	//* other component option *//

	const TransformComponent* GetTransform() const;

	//* serialization option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
