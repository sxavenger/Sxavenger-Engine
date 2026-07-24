#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "../Math/Vector2.h"
#include "../Math/Matrix4x4.h"

//* json
#include "../Adapter/Json/JsonSerializer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct RectTransform {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* convert methods *//

	Matrix4x4 ToMatrixPivot() const;

	Matrix4x4 ToMatrix() const;

	//* json option *//

	json Serialize() const;

	static RectTransform Deserialize(const json& data);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector2f scale     = kUnit2<float>;
	float rotate       = 0.0f;
	Vector2f translate = kOrigin2<float>;

	float priority = 0.0f; //!< z-order

	Vector2f pivot = kUnit2<float> / 2.0f;

private:
};
