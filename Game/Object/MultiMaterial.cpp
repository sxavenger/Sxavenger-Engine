#include "MultiMaterial.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MultiMaterial methods
////////////////////////////////////////////////////////////////////////////////////////////

void MultiMaterial::Init() {
	model_ = std::make_unique<Model>("resources/model", "multiMaterial.obj");

	// materailsの初期化
	for (uint32_t i = 0; i < kMaterialNum_; ++i) {
		materialResources_[i] = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevice(), 1);
		materialResources_[i]->Set(0, &materials_[i]);

		materials_[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materials_[i].uvTransform = Matrix4x4::MakeIdentity();
		materials_[i].lambertType = TYPE_LAMBERT;
		materials_[i].enableBlinnPhong = isEnableBlinnPhongs_[i];
		materials_[i].specPow = 100.0f;
	}
}

void MultiMaterial::EditorImGui(int identifier) {
	std::string title = "MultiMaterial##" + std::to_string(identifier);

	if (ImGui::TreeNode(title.c_str())) {

		transform_.SetImGuiCommand();

		for (uint32_t mi = 0; mi < kMaterialNum_; ++mi) {
			std::string materialTitle = "material##" + std::to_string(mi);

			if (ImGui::TreeNode(materialTitle.c_str())) {

				ImGui::ColorEdit4("color", &materials_[mi].color.x);
				ImGui::Spacing();
				ImGui::DragFloat2("scale", &uvTransforms_[mi].scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("rotate", &uvTransforms_[mi].rotate.z);
				ImGui::DragFloat2("translate", &uvTransforms_[mi].translate.x, 0.01f, -10.0f, 10.0f);

				materials_[mi].uvTransform = Matrix::MakeAffine(uvTransforms_[mi].scale, uvTransforms_[mi].rotate, uvTransforms_[mi].translate);

				if (ImGui::BeginCombo("lambertType", lambertItem_[materials_[mi].lambertType])) {
					for (int tj = 0; tj < kLambertTypeCount; ++tj) {
						bool isSelected = (tj == materials_[mi].lambertType);

						if (ImGui::Selectable(lambertItem_[tj], isSelected)) {
							materials_[mi].lambertType = tj;
						}
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("blinnPhong", &isEnableBlinnPhongs_[mi]);
				materials_[mi].enableBlinnPhong = isEnableBlinnPhongs_[mi];

				if (isEnableBlinnPhongs_[mi]) {
					ImGui::DragFloat("specPow", &materials_[mi].specPow, 1.0f, 0.0f, 1000.0f);
				}

				ImGui::TreePop();
			}
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

void MultiMaterial::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	for (uint32_t i = 0; i < model_->GetSize(); ++i) {
		model_->SetBuffers(commandList, i);

		commandList->SetGraphicsRootConstantBufferView(0, materialResources_[i]->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

		model_->SetTexture(4, commandList, i);

		model_->DrawCall(commandList, i);
	}
}

void MultiMaterial::Term() {
	model_.reset();

	for (uint32_t i = 0; i < kMaterialNum_; ++i) {
		materialResources_[i].reset();
	}
}
