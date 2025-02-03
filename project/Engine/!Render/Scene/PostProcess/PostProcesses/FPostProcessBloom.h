#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FPostProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessBloom class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcessBloom
	: public FPostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Vector2ui size;      //!< blur size
		float     threshold; //!< 閾値

		//* methods *//

		void Init() {
			size      = { 4, 4 };
			threshold = 1.0f;
		}
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcessBloom()          = default;
	virtual ~FPostProcessBloom() = default;

	void Init();

	//* process *//

	virtual void Process(const ProcessContext& context) override;

	virtual void SetImGuiCommand() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
