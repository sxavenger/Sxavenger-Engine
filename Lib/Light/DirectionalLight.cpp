#include <DirectionalLight.h>

#include <Matrix4x4.h>
#include <externals/imgui/imgui.h>

Directional::Directional(DxObject::Devices* devices) { Init(devices); }

Directional::~Directional() { Term(); }

void Directional::Init(DxObject::Devices* devices) {

	// resourceの確保
	resource_ = new DxObject::BufferPtrResource<DirectionalLight>(devices, 1);

	resource_->Set(0, &lightData_);

	lightData_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData_.direction = { 0.0f, 0.0f, 1.0f };
	lightData_.intensity = 1.0f;
}

void Directional::Term() {
	resource_.Release();
}

void Directional::UpdateImGui(const char* title, const char* lightName) {
	ImGui::Begin(title);

	if (ImGui::TreeNode(lightName)) {

		ImGui::ColorEdit4("color", &lightData_.color.x);

		static Vector3f rotate = origin;
		ImGui::DragFloat3("rotate", &rotate.x, 0.01f);

		lightData_.direction = Matrix::Transform({0.0f, 0.0f, 1.0f}, Matrix::MakeRotate(rotate));

		ImGui::SliderFloat("intensity", &lightData_.intensity, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();
}
