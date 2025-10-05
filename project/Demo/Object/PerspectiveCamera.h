#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class PerspectiveCamera
	: public GameObject {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Perspective enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Perspective : bool {
		FirstPerson,
		//ThirdPerson,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

	void Update() override;

	void Inspectable() override;

	//* setter *//

	void SetSubject(TransformComponent* subject) { subject_ = subject; }

	//* getter *//

	const Quaternion& GetRotation() const { return rotation_; }

	Perspective GetPerspective() const { return perspective_; }

	Vector3f GetForward() const;

	Vector3f GetPosition() const { return transform_->GetPosition(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input *//

	const KeyboardInput* keyboard_ = nullptr;
	const MouseInput* mouse_       = nullptr;

	//* component *//

	TransformComponent* transform_ = nullptr;
	CameraComponent* camera_       = nullptr;

	//* external *//

	TransformComponent* subject_ = nullptr;

	//* children *//

	std::unique_ptr<MonoBehaviour> light_;

	//* parameter *//

	Perspective perspective_ = Perspective::FirstPerson;

	SerializeParameter<Vector3f> offset_{ "offset", { 0.0f, 1.5f, 0.0f } };

	Vector3f point_      = {};
	Quaternion rotation_ = Quaternion::Identity();
	float distance_      = 0.0f;

	Vector2f angle_;

	//* parameter *//

	SerializeParameter<float> sensitivity_{ "sensitivity", 0.002f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InputFirstPerson();
	void UpdateFirstPersonView();
	void UpdateView();

};
