#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FAmbientProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib


//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcessAtmosphere class
////////////////////////////////////////////////////////////////////////////////////////////
class FAmbientProcessAtmosphere
	: public FAmbientProcess {
	//* 大気散乱
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Prameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		Color4f waveLength; //!< 波長
		Vector3f lightDir;
		float innerRadius;  //!< 内側の（惑星の）半径
		float outerRadius;  //!< 外側の（大気の）半径
		float kr;           //!< レイリー散乱係数
		float km;           //!< ミー散乱係数
		float eSun;         //!< 太陽の強さ
		float g;            //!< ミー散乱の位相関数のパラメーター
		float scaleDepth;  //!< 大気の平均密度が見つかる高度
		float exposure;

		void Reset() {
			lightDir    = {0.0f, 1.0f, 0.0f};
			waveLength  = { 0.68f, 0.55f, 0.44f, 1.0f };
			innerRadius = 8.0f;
			outerRadius = innerRadius + 16.0f;
			kr          = 0.0025f;
			km          = 0.001f;
			eSun        = 128.0f;
			g           = -0.95f;
			scaleDepth  = 0.25f;
			exposure    = 0.8f;
		}
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FAmbientProcessAtmosphere() = default;
	~FAmbientProcessAtmosphere() = default;

	void Init();

	//* process *//

	virtual void Process(const ProcessContext& context) override;

	//* debug *//

	virtual void SetImGuiCommand() override;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
