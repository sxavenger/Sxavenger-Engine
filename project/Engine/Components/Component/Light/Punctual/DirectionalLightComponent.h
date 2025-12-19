#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../../Transform/TransformComponent.h"

//* light
#include "../LightCommon.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Color3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
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

SXAVENGER_ENGINE_NAMESPACE_END
