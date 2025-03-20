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
#include <Lib/Geometry/Color3.h>
#include <Lib/Geometry/Color4.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Interface JsonSerialzer class
////////////////////////////////////////////////////////////////////////////////////////////
class IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual json OutputJson() const = 0;

	virtual void InputJson(const json& data) = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GeometryJsonSerializer class
////////////////////////////////////////////////////////////////////////////////////////////
struct GeometryJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static json ToJson(const Vector2f& v);
	static Vector2f JsonToVector2f(const json& data);

	static json ToJson(const Vector3f& v);
	static Vector3f JsonToVector3f(const json& data);

	static json ToJson(const Vector4f& v);
	static Vector4f JsonToVector4f(const json& data);

	static json ToJson(const Color4f& c);
	static Color4f JsonToColor4f(const json& data);

	static json ToJson(const Color3f& c);
	static Color3f JsonToColor3f(const json& data);

	static json ToJson(const Quaternion& q);
	static Quaternion JsonToQuaternion(const json& data);

	//!< todo templateでまとめる

private:
};

