#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Input/Input.h>
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerspectiveCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 1人称視点/3人称視点で操作可能なcameraクラス.
class PerspectiveCameraActor
	: public SXAVENGER_ENGINE GameObject {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Perspective enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Perspective : uint8_t {
		FirstPerson, //!< 1人称視点
		ThirdPerson, //!< 3人称視点
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PerspectiveCameraActor();
	~PerspectiveCameraActor() override = default;

	void Inspectable() override;

	bool Update();

	void UpdateTransform();

	//* actor option *//

	Perspective GetPerspective() const { return perspective_; }

	void SetPerspective(Perspective perspective);

	const Vector3f& GetPoint() const { return point_; }

	void SetPoint(const Vector3f& point) { point_ = point; }

	float GetDistance() const { return distance_; }

	void SetDistance(float distance) { distance_ = distance; }

	void SetAngle(const Vector2f& angle) { angle_ = angle; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* perspective *//

	Perspective perspective_ = Perspective::FirstPerson;

	//* parameter *//

	Vector3f point_      = kOrigin3<float>;
	Quaternion rotation_ = Quaternion::Identity();

	Vector2f angle_ = {};

	float distance_ = 0.0f;

	float speed_       = 4.0f;
	float sensitivity_ = 0.003f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* first person methods *//

	void AroundFirstPerson(const SXAVENGER_ENGINE KeyboardInput* keyboard, const SXAVENGER_ENGINE MouseInput* mouse);
	void MoveFirstPerson(const SXAVENGER_ENGINE KeyboardInput* keyboard, const SXAVENGER_ENGINE MouseInput* mouse);
	void FixMouseFirstPerson(const SXAVENGER_ENGINE KeyboardInput* keyboard, const SXAVENGER_ENGINE MouseInput* mouse);

	//* third person methods *//

	bool AroundThirdPerson(const SXAVENGER_ENGINE KeyboardInput* keyboard, const SXAVENGER_ENGINE MouseInput* mouse);
	bool MoveThirdPerson(const SXAVENGER_ENGINE KeyboardInput* keyboard, const SXAVENGER_ENGINE MouseInput* mouse);

	//* common methods *//

	bool ShouldUpdate() const;

};
