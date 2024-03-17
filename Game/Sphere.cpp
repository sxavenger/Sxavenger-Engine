#include "Sphere.h"

//=========================================================================================
// static variables
//=========================================================================================
const char* Sphere::lambertItems_[LambertType::kLambertTypeCount]
	= { "Lambert", "HalfLambert", "None" };
const char* Sphere::phongItems_[PhongType::kPhongTypeCount]
	= { "Phong", "BlinnPhong", "None" };

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere methods
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::Init() {
	data_ = DrawMethods::Sphere(1.0f, 16);

	matrixResource_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	matrixResource_->operator[](0).world = Matrix4x4::MakeIdentity();
	matrixResource_->operator[](0).wvp   = Matrix4x4::MakeIdentity();

	materialResource_ = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevicesObj(), 1);
	materialResource_->Set(0, &material_);

	material_.lambertType = LambertType::TYPE_LAMBERTNONE;
	material_.phongType   = PhongType::TYPE_PHONGNONE;
	material_.specPow     = 1.0f;
}

void Sphere::Draw() {
}

void Sphere::Term() {
	data_.Reset();
	matrixResource_.reset();
	materialResource_.reset();
}

void Sphere::SetOnImGui() {
	if (ImGui::TreeNode("sphere")) {

		if (ImGui::TreeNode("material")) {

			ImGui::ColorEdit4("color", &material_.color.r);

			if (ImGui::BeginCombo("lambertType", lambertItems_[material_.lambertType])) {
				for (int i = 0; i < LambertType::kLambertTypeCount; ++i) {
					bool isSelect = (material_.lambertType == i);

					if (ImGui::Selectable(lambertItems_[i], isSelect)) {
						material_.lambertType = i;
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("phongType", phongItems_[material_.phongType])) {
				for (int i = 0; i < PhongType::kPhongTypeCount; ++i) {
					bool isSelect = (material_.phongType == i);

					if (ImGui::Selectable(phongItems_[i], isSelect)) {
						material_.phongType = i;
					}
				}
				ImGui::EndCombo();
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
