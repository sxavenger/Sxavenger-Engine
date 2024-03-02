#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	model_ = new Model("resources/model", "teapot.obj");
}

void Teapot::EditorImGui(int identifier) {
	std::string title = "Teapot##" + std::to_string(identifier);

	SetImGui(title);
}

void Teapot::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	model_->SetTexture(4, commandList, 0);

	model_->DrawCall(commandList, 0);
}

void Teapot::Term() {
	model_.Release();
}
