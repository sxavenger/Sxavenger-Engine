#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* transform
#include "Transformation.h"

//* math
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix4x4.h"

//* json
#include "../Adapter/Json/JsonSerializer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TransformQuaternion structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformQuaternion {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* mathmatical methods *//

	Vector3f GetForward() const;

	//* convert methods *//

	Transformation ToTransformation() const;

	Matrix4x4 ToMatrix() const;

	//* json option *//

	json Serialize() const;

	static TransformQuaternion Deserialize(const json& data);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f scale     = kUnit3<float>;
	Quaternion rotate  = Quaternion::Identity();
	Vector3f translate = kOrigin3<float>;

private:
};
