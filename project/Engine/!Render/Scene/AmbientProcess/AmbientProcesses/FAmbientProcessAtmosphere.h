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
		Vector3f direction;
		float    intensity;

		void Init() {
			direction = { 0.0f, 1.0f, 0.0f };
			intensity = 20.0f;
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
