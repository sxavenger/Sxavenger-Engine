#include "ModelBehavior.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelBehavior::Init() {
	TransformComponent::CreateBuffer();

	mat_ = std::make_unique<DxObject::DimensionBuffer<Matrix4x4>>();
	(*mat_)[0] = Matrix4x4::Identity();
}

void ModelBehavior::SystemAttributeImGui() {
	if (ImGui::TreeNode("Transform Component")) {
		TransformComponent::SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3d* camera) {
}

void ModelBehavior::DrawAdaptive(_MAYBE_UNUSED const Camera3d* camera) {
}

void ModelBehavior::DrawLateAdaptive(_MAYBE_UNUSED const Camera3d* camera) {
}
