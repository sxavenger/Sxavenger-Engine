#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Module/Parameter/SerializeParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class PerspectiveCamera
	: public SxxEngine::GameObject {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Perspective enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Person {
		First,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Awake();

	void Update();

	void Inspectable() override;

	//* option *//

	Vector3f CalculateMoveFront(const Vector3f& vector) const;

	Quaternion CalculateForward() const;

	//* setter *//

	void SetAround(const Vector2f& around) { around_ = around; }

	void SetTarget(const SxxEngine::BehaviourAddress& target) { target_ = target; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	SxxEngine::SerializeParameter<float> sensitivity_{ "sensitivity", 0.002f };

	SxxEngine::SerializeParameter<Vector3f> offset_{ "offset", { 0.0f, 2.0f, 0.0f } };
	//!< 1人称視点オフセット

	//* variable *//

	Person person_ = Person::First;

	Vector3f point_      = {};
	Quaternion rotation_ = Quaternion::Identity();
	Vector2f angle_      = kOrigin2<float>;
	float distance_      = 12.0f;

	Vector2f around_ = {};

	SxxEngine::BehaviourAddress target_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateFirstPerson();

	void UpdateView();

};
