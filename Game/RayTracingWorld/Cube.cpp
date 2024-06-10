#include "Cube.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Cube class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Cube::Init(

	DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
	DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer) {

	blas_ = std::make_unique<DxrObject::BottomLevelAS>();
	blas_->Create(
		vertices, indices,
		verticesStructuredBuffer, indicesStructuredBuffer,
		L"cube"
	);

	InitMaterial();

}

void Cube::SetOnImGui(int id) {

	std::string label = "cube##" + std::to_string(id);

	if (ImGui::TreeNode(label.c_str())) {

		// transformの更新
		ImGui::Text("transform");
		transform_.SetImGuiCommand();
		worldMatrix_ = transform_.SetMatrix();

		// materialの更新
		ImGui::Text("material");
		ImGui::ColorEdit4("color", &material_.color.r);

		blas_->SetBuffer(3, materialBuffer_->GetGPUVirtualAddress()); //!< HACK: 本来の機能の役目を果たしてない

		if (ImGui::Button("Delete")) {
			isDelete_ = true;
		}

		ImGui::TreePop();
	}
}
