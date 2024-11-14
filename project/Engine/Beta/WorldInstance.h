#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/Transform.h>

////////////////////////////////////////////////////////////////////////////////////////////
// WorldInstance class
////////////////////////////////////////////////////////////////////////////////////////////
class WorldInstance {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	WorldInstance()  { Create(); }
	~WorldInstance() = default;

	void Create();

	const QuaternionTransformBuffer& GetTransformBuffer() const { return transform_; }
	QuaternionTransformBuffer& GetTransformBuffer() { return transform_; }

protected:

	//=========================================================================================
	// private variables
	//=========================================================================================

	QuaternionTransformBuffer transform_;

};