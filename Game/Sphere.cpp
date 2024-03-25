#include "Sphere.h"

//=========================================================================================
// static variables
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Sphere::Init() {
	data_ = DrawMethods::Sphere(1.0f, 16);

	matrixResource_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	matrixResource_->operator[](0).world = Matrix4x4::MakeIdentity();
	matrixResource_->operator[](0).worldInverseTranspose = Matrix4x4::MakeIdentity();
	matrixResource_->operator[](0).wvp = Matrix4x4::MakeIdentity();

	materialResource_ = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevicesObj(), 1);
	materialResource_->Set(0, &material_);

	material_.lambertType = LambertType::TYPE_LAMBERTNONE;
	material_.phongType   = PhongType::TYPE_PHONGNONE;
	material_.specPow     = 100.0f;
}

void Sphere::Draw(Light* light) {

	// commandListの取り出し
	ID3D12GraphicsCommandList* commandList = MyEngine::GetCommandList();

	MyEngine::SetPipelineType(TEXTURE);
	MyEngine::SetBlendMode(kBlendModeNormal);
	MyEngine::SetPipelineState();

	const auto vertexBufferViwe = data_.vertex->GetVertexBufferView();
	const auto indexBufferView  = data_.index->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferViwe);
	commandList->IASetIndexBuffer(&indexBufferView);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, light->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(4, MyEngine::GetTextureHandleGPU("resources/monsterBall.png"));

	commandList->DrawIndexedInstanced(data_.index->GetSize(), 1, 0, 0, 0);
}

void Sphere::Term() {
	data_.Reset();
	matrixResource_.reset();
	materialResource_.reset();
}

void Sphere::SetOnImGui() {
	if (ImGui::TreeNode("sphere")) {

		transform_.SetImGuiCommand();
		material_.SetImGuiCommand();

		ImGui::TreePop();
	}

	Matrix4x4 worldMatrix = Matrix::MakeAffine(transform_.scale, transform_.rotate, transform_.translate);

	matrixResource_->operator[](0).world = worldMatrix;
	matrixResource_->operator[](0).worldInverseTranspose = Matrix::Transpose(Matrix::Inverse(worldMatrix));
	matrixResource_->operator[](0).wvp = worldMatrix * MyEngine::camera3D_->GetViewProjectionMatrix();
}
