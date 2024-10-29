#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxBufferResource.h>

//* lib
#include <lib/Geometry/Vector2.h>
#include <lib/Geometry/Vector4.h>
#include <lib/Geometry/Matrix4x4.h>

//* c++
#include <memory>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransform structure
////////////////////////////////////////////////////////////////////////////////////////////
struct UVTransform {
	Vector2f scale;
	float    rotate;
	Vector2f translate;

	UVTransform()
		: scale({ 1.0f, 1.0f }), rotate(0.0f), translate({ 0.0f, 0.0f }) {
	}

	Matrix4x4 ToMatrix() const {
		return Matrix::MakeAffine({ scale.x, scale.y, 1.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, 0.0f });
	}

	void SetImGuiCommand() {
		ImGui::DragFloat2("scale",     &scale.x, 0.01f);
		ImGui::SliderAngle("rotate",   &rotate);
		ImGui::DragFloat2("translate", &translate.x, 0.01f);
	}
};