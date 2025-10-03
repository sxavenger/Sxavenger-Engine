#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Actor/PivotCameraActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PivotCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class PivotCamera
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	//* inspectable *//

	void Inspectable() override;

	//* setter *//

	void SetPoint(const Vector3f& point) { camera_->SetPoint(point + offset_); }

	//* getter *//

	const Quaternion& GetRotation() const { return camera_->GetRotation(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const KeyboardInput* keyboard_ = nullptr;
	const GamepadInput* gamepad_   = nullptr;

	//* camera *//

	std::unique_ptr<PivotCameraActor> camera_;

	//* parameter *//

	Vector2f angle_ = { 0.0f, kPi / 16.0f };
	Vector2f speed_;

	float maxSpeed_ = 0.1f;

	Vector3f offset_ = { 0.0f, 3.0f, -0.4f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f GetInputDirection() const;

	void Rotation();

};
