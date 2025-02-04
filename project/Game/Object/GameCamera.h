#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class GameCamera
	: public ACineCameraActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameCamera()  = default;
	~GameCamera() = default;

	void Init();

	void Term();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const GamepadInput* gamepad_ = nullptr;

	//* parameter *//

	Vector2f rotationDelta_ = { 0.02f, 0.01f };

	APivotCameraActor::Parameter parameter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateGamepad();

};
