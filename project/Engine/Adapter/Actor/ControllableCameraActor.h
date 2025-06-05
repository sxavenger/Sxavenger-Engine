#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ControllableCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ControllableCameraActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const MouseInput* mouse_       = nullptr;

	//* parameter *//

	Vector2f angle_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool CheckNeedUpdate();

	void Around();

	void Move();

	void FixMouse();

};
