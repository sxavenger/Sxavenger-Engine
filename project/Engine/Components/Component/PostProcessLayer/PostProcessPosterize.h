#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessPosterize class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 階調を減らすポスタリゼーションのポストプロセス
class PostProcessPosterize
	: public BasePostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint32_t {
		Ceil,  //!< 切り上げ [std::ceil](https://cpprefjp.github.io/reference/cmath/ceil.html)
		Floor, //!< 切り捨て [std::floor](https://cpprefjp.github.io/reference/cmath/floor.html)
		Round, //!< 四捨五入 [std::round](https://cpprefjp.github.io/reference/cmath/round.html)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief このエフェクトのパラメータを保持する構造体
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static Parameter Default();

		//=========================================================================================
		// public variables
		//=========================================================================================

		float coordinateLevel;
		Mode coordinateMode;

		float colorLevel;
		Mode colorMode;
		float colorBrightnessMin;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXQueueContext* context, const ProcessInfo& info) override;

	void ShowInspectorImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::ConstantBuffer<Parameter> parameter_;

};

SXAVENGER_ENGINE_NAMESPACE_END
