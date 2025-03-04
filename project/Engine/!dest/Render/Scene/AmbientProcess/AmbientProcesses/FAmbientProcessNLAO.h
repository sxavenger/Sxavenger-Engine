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
// FAmbientProcessNLAO class
////////////////////////////////////////////////////////////////////////////////////////////
class FAmbientProcessNLAO
	: public FAmbientProcess {
	//* 自作式のAmbient Occlusion. (Normal Lambert Ambient Occlusion)
	//* hack: まだシステムとしては未完成
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Prameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Vector2ui samples;
		Vector2ui size;
		float radius;
		float strength;

		//* methods *//

		void Init() {
			samples  = { 1, 1 };
			size     = { 4, 4 };
			radius   = 0.8f;
			strength = 1.0f;
		}
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FAmbientProcessNLAO()  = default;
	~FAmbientProcessNLAO() = default;

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
