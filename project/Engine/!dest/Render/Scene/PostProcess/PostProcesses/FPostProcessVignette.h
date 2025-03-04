#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FPostProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessVignette class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcessVignette
	: public FPostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		Color4f color;
		float offset;
		float power;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcessVignette()          = default;
	virtual ~FPostProcessVignette() = default;

	void Init();

	//* process *//

	void Process(const ProcessContext& context) override;

	//* debug *//

	void SetImGuiCommand() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
