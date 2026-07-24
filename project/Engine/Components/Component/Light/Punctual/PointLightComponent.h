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
// PointLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 一点から全方向へ照らす点光源のcomponent
class PointLightComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPUへ転送する点光源パラメータ(色/強度/減衰半径/影設定)
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 各パラメータを既定値で初期化する
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

	PointLightComponent(EntityBehaviour* behaviour);
	~PointLightComponent() override = default;

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* component option *//

	//! @brief ライトパラメータ定数バッファのGPU仮想アドレスを取得する
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//! @brief 光の強度の単位系を設定する
	//! @param[in] unit 設定する単位
	void SetUnit(LightCommon::Units unit);

	//! @brief ライトパラメータを取得する
	//! @return Parameterへの参照
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
