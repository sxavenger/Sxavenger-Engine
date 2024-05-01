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

struct TransformationMatrix {
	Matrix4x4 wvp;
	Matrix4x4 world;
	Matrix4x4 worldInverseTranspose;

	/*void SetTransform(const Transform& transform, const Matrix4x4& viewProjMatrix) {
		world = Matrix::MakeAffine(transform.scale, transform.rotate, transform.translate);
		worldInverseTranspose = Matrix::Transpose(Matrix::Inverse(world));
		wvp = world * viewProjMatrix;
	}*/
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
	Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 uvTransform = Matrix4x4::MakeIdentity();
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

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ResMeshlet {
	uint32_t vertexOffset; //!< 頂点番号のオフセット
	uint32_t vertexCount;  //!< 頂点数
	uint32_t primitiveOffset; //!< プリミティブ番号のオフセット
	uint32_t primitiveCount;  //!< プリミティブオフセット
};

struct ResPrimitiveIndex {
	uint32_t index0 : 10;
	uint32_t index1 : 10;
	uint32_t index2 : 10;
	uint32_t reserved : 2;
};

struct ResMesh {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> index;
	uint32_t materialId;

	std::vector<ResMeshlet> meshlet;
	std::vector<uint32_t> uniqueVertexIndices;
	std::vector<ResPrimitiveIndex> primitiveIndices;
};