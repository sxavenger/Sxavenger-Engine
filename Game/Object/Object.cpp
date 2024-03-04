#include "Object.h"

//=========================================================================================
// init variables
//=========================================================================================
const char* Object::lambertItem_[kLambertTypeCount] = { "lambert", "halfLambert", "none" };

////////////////////////////////////////////////////////////////////////////////////////////
// Object methods
////////////////////////////////////////////////////////////////////////////////////////////

void Object::InitResource() {
	// matrixResource
	matrixResource_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevice(), 1);
	matrixResource_->operator[](0).world = worldMatrix_;
	matrixResource_->operator[](0).wvp = worldMatrix_ * MyEngine::camera3D_->GetViewProjectionMatrix();

	// materialResource_
	materialResource_ = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevice(), 1);
	materialResource_->Set(0, &material_);

	material_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	material_.uvTransform = Matrix4x4::MakeIdentity();
	material_.lambertType = TYPE_LAMBERT;
	material_.enableBlinnPhong = isEnableBlinnPhong_;
	material_.specPow = 100.0f;
}

void Object::TermResource() {
	matrixResource_.reset();
	materialResource_.reset();
}

void Object::SetImGui(const std::string& title) {
	if (ImGui::TreeNode(title.c_str())) {

		transform_.SetImGuiCommand();

		if (ImGui::TreeNode("material")) {

			ImGui::ColorEdit4("color", &material_.color.x);
			ImGui::Spacing();
			ImGui::DragFloat2("scale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("rotate", &uvTransform_.rotate.z);
			ImGui::DragFloat2("translate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);

			material_.uvTransform = Matrix::MakeAffine(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);

			if (ImGui::BeginCombo("lambertType", lambertItem_[material_.lambertType])) {
				for (int i = 0; i < kLambertTypeCount; ++i) {
					bool isSelected = (i == material_.lambertType);

					if (ImGui::Selectable(lambertItem_[i], isSelected)) {
						material_.lambertType = i;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("blinnPhong", &isEnableBlinnPhong_);
			material_.enableBlinnPhong = isEnableBlinnPhong_;

			if (isEnableBlinnPhong_) {
				ImGui::DragFloat("specPow", &material_.specPow, 1.0f, 0.0f, 1000.0f);
			}

			ImGui::TreePop();
		}

		if (ImGui::Button("delete")) {
			isDelete_ = true;
			ImGui::TreePop();
			return;
		}

		// 再計算
		worldMatrix_ = Matrix::MakeAffine(transform_.scale, transform_.rotate, transform_.translate);
		matrixResource_->operator[](0).world = worldMatrix_;
		matrixResource_->operator[](0).wvp = worldMatrix_ * MyEngine::camera3D_->GetViewProjectionMatrix();

		

		ImGui::TreePop();
	}
}
