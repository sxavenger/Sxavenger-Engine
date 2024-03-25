#include "Terrain.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Light.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Terrain class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Terrain::Init() {
	// model
	model_ = std::make_unique<Model>("./resources/model", "terrain.obj");

	matrixResource_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	matrixResource_->operator[](0).world = Matrix4x4::MakeIdentity();
	matrixResource_->operator[](0).worldInverseTranspose = Matrix4x4::MakeIdentity();
	matrixResource_->operator[](0).wvp = Matrix4x4::MakeIdentity();

	materialResource_ = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevicesObj(), 1);
	materialResource_->Set(0, &material_);

	material_.lambertType = LambertType::TYPE_LAMBERTNONE;
	material_.phongType = PhongType::TYPE_PHONGNONE;
	material_.specPow = 100.0f;
}

void Terrain::Draw(Light* light) {

	// commandListの取り出し
	ID3D12GraphicsCommandList* commandList = MyEngine::GetCommandList();

	MyEngine::SetBlendMode(BlendMode::kBlendModeNormal);
	MyEngine::SetPipelineType(PipelineType::TEXTURE);
	MyEngine::SetPipelineState();

	for (uint32_t i = 0; i < model_->GetSize(); ++i) {
		model_->SetBuffers(commandList, i);

		commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(2, light->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());
		model_->SetTexture(4, commandList, i);

		model_->DrawCall(commandList, i, 1);
	}

}

void Terrain::Term() {
	model_.reset();
	matrixResource_.reset();
	materialResource_.reset();
}

void Terrain::SetOnImGui() {
	if (ImGui::TreeNode("terrain")) {

		transform_.SetImGuiCommand();
		material_.SetImGuiCommand();

		ImGui::TreePop();
	}

	Matrix4x4 worldMatrix = Matrix::MakeAffine(transform_.scale, transform_.rotate, transform_.translate);

	matrixResource_->operator[](0).world = worldMatrix;
	matrixResource_->operator[](0).worldInverseTranspose = Matrix::Transpose(Matrix::Inverse(worldMatrix));
	matrixResource_->operator[](0).wvp = worldMatrix * MyEngine::camera3D_->GetViewProjectionMatrix();

}
