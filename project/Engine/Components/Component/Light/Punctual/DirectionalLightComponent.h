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
// DirectionalLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 一定方向から照らす平行光源(太陽光等)のcomponent
class DirectionalLightComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPUへ転送する平行光源パラメータ(色/強度/影設定)
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
		float   intensity; //!< [lux]

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectionalLightComponent(EntityBehaviour* behaviour);
	~DirectionalLightComponent() override = default;

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* component option *//

	//! @brief ライトパラメータ定数バッファのGPU仮想アドレスを取得する
	//! @return 定数バッファのGPU仮想アドレス
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//! @brief ライトパラメータを取得する
	//! @return Parameterへの参照
	const Parameter& GetParameter() const;
	//! @brief ライトパラメータを取得する (非const版)
	//! @return Parameterへの参照
	Parameter& GetParameter();

	//* behaviour component option *//

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

	//* serialization option *//

	//! @brief このcomponentの状態をjsonへシリアライズする (BaseComponentのoverride)
	//! @return シリアライズしたjson
	json ParseToJson() const override;

	//! @brief jsonからこのcomponentの状態を復元する (BaseComponentのoverride)
	//! @param[in] data 入力元のjson
	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

};

SXAVENGER_ENGINE_NAMESPACE_END
