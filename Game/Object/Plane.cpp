#include "Plane.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Plane methods
////////////////////////////////////////////////////////////////////////////////////////////

void Plane::Init() {
	model_ = std::make_unique<Model>("resources/model", "plane2.obj");
	/*model_ = new Model("resources/model", "cube.obj");*/
}

void Plane::EditorImGui(int identifier) {
	std::string title = "Plane##" + std::to_string(identifier);

	SetImGui(title);
}

void Plane::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	model_->SetTexture(4, commandList, 0);
	
	model_->DrawCall(commandList, 0);
}

void Plane::Term() {
	model_.reset();
}
