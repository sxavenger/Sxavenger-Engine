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

}

void Cube::SetOnImGui(int id) {

	std::string label = "cube##" + std::to_string(id);

	if (ImGui::TreeNode(label.c_str())) {

		transform_.SetImGuiCommand();
		worldMatrix_ = transform_.SetMatrix();

		if (ImGui::Button("Delete")) {
			isDelete_ = true;
		}

		ImGui::TreePop();
	}
}
