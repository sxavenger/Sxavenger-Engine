#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FPostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/!Render/Scene/Actor/Camera/ACameraActor.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessDoF class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessDoF
	: public FPostProcess { //!< 被写界深度
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Vector2ui strength;
		float     focus;
		float     f;
		uint32_t  isDebugView; //!< デバッグ表示

		//* methods *//

		void Init() {
			strength    = { 8, 8 };
			focus       = 0.0f;
			f           = 0.0f;
			isDebugView = false;
		}

		// hack: parameterの不足
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessDoF()          = default;
	virtual ~FProcessDoF() = default;

	void Init();

	//* process *//

	virtual void Process(const FPostProcess::ProcessContext& context) override;

	//* debug *//

	virtual void SetImGuiCommand() override;

	//* getter *//

	const Parameter& GetParameter() const { return parameter_->At(0); }
	Parameter& GetParameter() { return parameter_->At(0); }

	//* setter *//

	void SetFocus(const Vector3f& position, const ACameraActor* camera);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
