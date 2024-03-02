#include "Suzanne.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Suzanne methods
////////////////////////////////////////////////////////////////////////////////////////////

void Suzanne::Init() {
	model_ = new Model("resources/model", "Suzanne.obj");
}

void Suzanne::EditorImGui(int identifier) {
	std::string title = "Suzanne##" + std::to_string(identifier);

	SetImGui(title);
}

void Suzanne::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	model_->DrawCall(commandList, 0);
}

void Suzanne::Term() {
	model_.Release();
}
