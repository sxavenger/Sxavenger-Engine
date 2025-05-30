#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PivotCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
class PivotCameraActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Update();

	//* inspectable *//

	void Inspectable() override;

	//* setter *//

	void SetPoint(const Vector3f& point) { point_ = point; }

	void SetRotation(const Quaternion& rotation) { rotation_ = rotation; }

	void SetDistance(float distance) { distance_ = distance; }

	//* getter *//

	const Quaternion& GetRotation() const { return rotation_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* parameter *//

	Vector3f point_      = {};
	Quaternion rotation_ = {};
	float distance_      = 12.0f;

};
