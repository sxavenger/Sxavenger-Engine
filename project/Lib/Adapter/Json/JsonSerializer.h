#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include "JsonHandler.h"

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

	virtual json PerseToJson() const = 0;

	virtual void InputJson(const json& data) = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializePrimitive concept
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
concept JsonSerializePrimitive = std::integral<T> || std::floating_point<T> || std::same_as<T, std::string>;

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter {
public:

	static json Serialize(const T& value) {
		static_assert(false, "JsonSerializeFormatter::Serialize() : Unsupported type.");
	}

	static T Deserialize(const json& j) {
		static_assert(false, "JsonSerializeFormatter::Deserialize() : Unsupported type.");
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for primitive types
////////////////////////////////////////////////////////////////////////////////////////////
template <JsonSerializePrimitive T>
struct JsonSerializeFormatter<T> {
public:

	static json Serialize(const T& value) {
		return value;
	}

	static T Deserialize(const json& j) {
		return j.get<T>();
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Vector2
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Vector2<T>> {
public:

	static json Serialize(const Vector2<T>& value) {
		return {
			{"x", JsonSerializeFormatter<T>::Serialize(value.x)},
			{"y", JsonSerializeFormatter<T>::Serialize(value.y)}
		};
	}

	static Vector2<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("x")),
			JsonSerializeFormatter<T>::Deserialize(j.at("y"))
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Vector3
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Vector3<T>> {
public:

	static json Serialize(const Vector3<T>& value) {
		return {
			{"x", JsonSerializeFormatter<T>::Serialize(value.x)},
			{"y", JsonSerializeFormatter<T>::Serialize(value.y)},
			{"z", JsonSerializeFormatter<T>::Serialize(value.z)}
		};
	}

	static Vector3<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("x")),
			JsonSerializeFormatter<T>::Deserialize(j.at("y")),
			JsonSerializeFormatter<T>::Deserialize(j.at("z"))
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Vector4
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Vector4<T>> {
public:

	static json Serialize(const Vector4<T>& value) {
		return {
			{"x", JsonSerializeFormatter<T>::Serialize(value.x)},
			{"y", JsonSerializeFormatter<T>::Serialize(value.y)},
			{"z", JsonSerializeFormatter<T>::Serialize(value.z)},
			{"w", JsonSerializeFormatter<T>::Serialize(value.w)}
		};
	}

	static Vector4<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("x")),
			JsonSerializeFormatter<T>::Deserialize(j.at("y")),
			JsonSerializeFormatter<T>::Deserialize(j.at("z")),
			JsonSerializeFormatter<T>::Deserialize(j.at("w"))
		};
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Quaternion
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct JsonSerializeFormatter<Quaternion> {
public:

	static json Serialize(const Quaternion& value) {
		return {
			{"x", JsonSerializeFormatter<float>::Serialize(value.imaginary.x) },
			{"y", JsonSerializeFormatter<float>::Serialize(value.imaginary.y) },
			{"z", JsonSerializeFormatter<float>::Serialize(value.imaginary.z) },
			{"w", JsonSerializeFormatter<float>::Serialize(value.w) },
		};
	}

	static Quaternion Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<float>::Deserialize(j.at("x")),
			JsonSerializeFormatter<float>::Deserialize(j.at("y")),
			JsonSerializeFormatter<float>::Deserialize(j.at("z")),
			JsonSerializeFormatter<float>::Deserialize(j.at("w"))
		};
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Color3(integral)
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Color3Integral<T>> {
public:

	static json Serialize(const Color3Integral<T>& value) {
		return {
			{"r", JsonSerializeFormatter<T>::Serialize(value.r)},
			{"g", JsonSerializeFormatter<T>::Serialize(value.g)},
			{"b", JsonSerializeFormatter<T>::Serialize(value.b)}
		};
	}

	static Color3Integral<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("r")),
			JsonSerializeFormatter<T>::Deserialize(j.at("g")),
			JsonSerializeFormatter<T>::Deserialize(j.at("b"))
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Color3(floating-point)
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Color3floating<T>> {
public:

	static json Serialize(const Color3floating<T>& value) {
		return {
			{"r", JsonSerializeFormatter<T>::Serialize(value.r)},
			{"g", JsonSerializeFormatter<T>::Serialize(value.g)},
			{"b", JsonSerializeFormatter<T>::Serialize(value.b)}
		};
	}

	static Color3floating<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("r")),
			JsonSerializeFormatter<T>::Deserialize(j.at("g")),
			JsonSerializeFormatter<T>::Deserialize(j.at("b"))
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Color4(integral)
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Color4Integral<T>> {
public:

	static json Serialize(const Color4Integral<T>& value) {
		return {
			{"r", JsonSerializeFormatter<T>::Serialize(value.r)},
			{"g", JsonSerializeFormatter<T>::Serialize(value.g)},
			{"b", JsonSerializeFormatter<T>::Serialize(value.b)},
			{"a", JsonSerializeFormatter<T>::Serialize(value.a)}
		};
	}

	static Color4Integral<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("r")),
			JsonSerializeFormatter<T>::Deserialize(j.at("g")),
			JsonSerializeFormatter<T>::Deserialize(j.at("b")),
			JsonSerializeFormatter<T>::Deserialize(j.at("a"))
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonSerializeFormatter structure for Color4(floating-point)
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct JsonSerializeFormatter<Color4floating<T>> {
public:

	static json Serialize(const Color4floating<T>& value) {
		return {
			{"r", JsonSerializeFormatter<T>::Serialize(value.r)},
			{"g", JsonSerializeFormatter<T>::Serialize(value.g)},
			{"b", JsonSerializeFormatter<T>::Serialize(value.b)},
			{"a", JsonSerializeFormatter<T>::Serialize(value.a)}
		};
	}

	static Color4floating<T> Deserialize(const json& j) {
		return {
			JsonSerializeFormatter<T>::Deserialize(j.at("r")),
			JsonSerializeFormatter<T>::Deserialize(j.at("g")),
			JsonSerializeFormatter<T>::Deserialize(j.at("b")),
			JsonSerializeFormatter<T>::Deserialize(j.at("a"))
		};
	}
};
