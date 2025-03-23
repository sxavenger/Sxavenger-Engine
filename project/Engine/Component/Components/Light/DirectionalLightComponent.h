#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* light
#include "LightCommon.h"

//* lib
#include <Lib/Geometry/Color3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectionalLightComponent final
	: public BaseComponent, public LightCommon {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter class
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

		Color3f color;     //!< color
		float   intensity; //!< intensity [0.0f ~ 1.0f]
		// todo: 輝度単位をluxで設定

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectionalLightComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	virtual ~DirectionalLightComponent() = default;

	virtual void ShowComponentInspector() override;

	void Init();

	//* buffer option *//

	void CreateParameterBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetParameterBufferAddress() const;

	//* other component option *//

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
