#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Module/GameObject/GameObject.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ControllableCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief keyboard/mouseで操作可能なcameraクラス.
class ControllableCameraActor
	: public SXAVENGER_ENGINE GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ControllableCameraActor();
	~ControllableCameraActor() override = default;

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input *//

	const SXAVENGER_ENGINE KeyboardInput* keyboard_ = nullptr;
	const SXAVENGER_ENGINE MouseInput* mouse_       = nullptr;

	//* parameter *//

	Vector2f angle_;

	float speed_ = 1.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool CheckNeedUpdate();

	void Around();

	void Move();

	void FixMouse();

};
