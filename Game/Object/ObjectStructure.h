#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

// imgui
#include <externals/imgui/imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexData {
	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;
};

struct TransformationMatrix {
	Matrix4x4 wvp;
	Matrix4x4 world;
};

struct Transform {
	Vector3f scale;
	Vector3f rotate;
	Vector3f translate;

	Transform() :
		scale(unitVector),
		rotate(origin),
		translate(origin) {
	}

	void SetImGuiCommand(float granularity = 0.01f) {
		ImGui::DragFloat3("scale", &scale.x, granularity);
		ImGui::DragFloat3("rotate", &rotate.x, granularity);
		ImGui::DragFloat3("translate", &translate.x, granularity);
	}
};

enum LambertType {
	TYPE_LAMBERT,
	TYPE_HALF_LAMBERT,
	TYPE_NONE,

	kLambertTypeCount
};

struct Material {
	Vector4f color;
	Matrix4x4 uvTransform = Matrix4x4::MakeIdentity();
	int lambertType; //!< LambertType参照
	int enableBlinnPhong;
	float specPow;

//private:
//	float padding[3];
//public:
};