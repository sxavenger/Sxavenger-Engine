#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include <Lib/Adapter/Json/JsonAdapter.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Quaternion.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Interface JsonSerialzer class
////////////////////////////////////////////////////////////////////////////////////////////
class IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual json OutputJson() = 0;

	virtual void InputJson(const json& data) = 0;

	//* serialize option *//

	static json ToJson(const Vector2f& v);
	static Vector2f ToVector2f(const json& data);

	static json ToJson(const Vector3f& v);
	static Vector3f ToVector3f(const json& data);

	static json ToJson(const Vector4f& v);
	static json ToJson(const Color4f& c);

	static json ToJson(const Quaternion& q);
	static Quaternion ToQuaternion(const json& data);

};