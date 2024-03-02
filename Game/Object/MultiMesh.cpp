#include "MultiMesh.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MultiMesh class
////////////////////////////////////////////////////////////////////////////////////////////

void MultiMesh::Init() {
	model_ = new Model("resources/model", "multiMesh.obj");
}

void MultiMesh::EditorImGui(int identifier) {
	std::string title = "MultiMesh##" + std::to_string(identifier);

	SetImGui(title);
}

void MultiMesh::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	for (uint32_t i = 0; i < model_->GetModelSize(); ++i) {
		model_->SetBuffers(commandList, i);

		commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

		model_->SetTexture(4, commandList, i);

		model_->DrawCall(commandList, i);
	}
}

void MultiMesh::Term() {
	model_.Release();
}
