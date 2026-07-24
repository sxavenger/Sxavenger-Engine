#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* light
#include "../LightCommon.h"

//* component
#include "../../BaseComponent.h"
#include "../../Transform/TransformComponent.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Math/Color3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RectLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class RectLightComponent
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
		Vector2f source;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RectLightComponent(EntityBehaviour* behaviour);
	~RectLightComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	void SetUnit(LightCommon::Units unit);

	const Parameter& GetParameter() const;

	//* behaviour component option *//

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

	//* serialization option *//

	json ParseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

};

SXAVENGER_ENGINE_NAMESPACE_END
