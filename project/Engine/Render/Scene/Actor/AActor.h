#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// AActor class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformComponentDummy {
	//* 3D Object で Transformを持っているモノをActorとして扱う
public:

	void UpdateMatrix() { mat_ = transform_.ToMatrix(); }

	const Matrix4x4& GetMatrix() const { return mat_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	QuaternionTransform transform_;
	Matrix4x4 mat_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AActor
	: public TransformComponentDummy {
	//* 3D Object で Transformを持っているモノをActorとして扱う
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AActor()          = default;
	virtual ~AActor() = default;

	virtual void InspectorImGui() {}
	//!< Inspector表示用のImGui

	//* getter *//

	const std::string& GetName() const { return name_; }
	std::string& GetName() { return name_; }

	bool IsActive() const { return isActive_; }

	//* setter *//

	void SetActive(bool isActive) { isActive_ = isActive; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "actor";

	bool isActive_ = true;

};
