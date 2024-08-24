#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Quaternion.h>
#include <Matrix4x4.h>

// imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexData {
	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;

	void Init() {
		position = { 0.0f, 0.0f, 0.0f, 1.0f };
		texcoord = { 0.0f, 0.0f };
		normal   = { 0.0f, 0.0f, 0.0f };
	}
};

enum LambertType {
	TYPE_LAMBERT,
	TYPE_HALF_LAMBERT,
	TYPE_LAMBERTNONE,

	kLambertTypeCount
};

static const char* lambertItems_[LambertType::kLambertTypeCount]
	= { "Lambert", "HalfLambert", "None" };

enum PhongType {
	TYPE_PHONG,
	TYPE_BLINNPHONG,
	TYPE_PHONGNONE,

	kPhongTypeCount
};
static const char* phongItems_[PhongType::kPhongTypeCount]
	= { "Phong", "BlinnPhong", "None" };

struct Material {
	Color4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 uvTransform = Matrix4x4::Identity();
	int lambertType; //!< LambertType参照
	int phongType;   //!< phongType参照
	float specPow;

	void SetImGuiCommand() {
		if (ImGui::TreeNode("material")) {

			ImGui::ColorEdit4("color", &color.r);

			if (ImGui::BeginCombo("lambertType", lambertItems_[lambertType])) {
				for (int i = 0; i < LambertType::kLambertTypeCount; ++i) {
					bool isSelect = (lambertType == i);

					if (ImGui::Selectable(lambertItems_[i], isSelect)) {
						lambertType = i;
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("phongType", phongItems_[phongType])) {
				for (int i = 0; i < PhongType::kPhongTypeCount; ++i) {
					bool isSelect = (phongType == i);

					if (ImGui::Selectable(phongItems_[i], isSelect)) {
						phongType = i;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::DragFloat("specPow", &specPow, 0.1f);

			ImGui::TreePop();
		}
	}

};