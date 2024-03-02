#include "Sprite.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sprite methods
////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Init() {
	// vertexResource
	vertexResource_ = new DxObject::BufferResource<VertexData>(MyEngine::GetDevice(), 4);
	vertexBufferView_ = vertexResource_->GetVertexBufferView();

	vertexResource_->operator[](0).position = { 0.0f, 360.0f, 0.0f, 1.0f };   // leftBottom
	vertexResource_->operator[](0).texcoord = { 0.0f, 1.0f };
	vertexResource_->operator[](1).position = { 0.0f, 0.0f, 0.0f, 1.0f };     // leftTop
	vertexResource_->operator[](1).texcoord = { 0.0f, 0.0f };
	vertexResource_->operator[](2).position = { 640.0f, 360.0f, 0.0f, 1.0f }; // rightBottom
	vertexResource_->operator[](2).texcoord = { 1.0f, 1.0f };
	vertexResource_->operator[](3).position = { 640.0f, 0.0f, 0.0f, 1.0f };   // rightTop
	vertexResource_->operator[](3).texcoord = { 1.0f, 0.0f };

	// indexResource
	indexResource_ = new DxObject::IndexBufferResource(MyEngine::GetDevice(), vertexResource_.Get());
	indexBufferView_ = indexResource_->GetIndexBufferView();

	indexResource_->SetTriangle(1, { 0, 1, 2 });
	indexResource_->SetTriangle(2, { 1, 3, 2 });

	// matrixResource
	matrixResource_->operator[](0).world = worldMatrix_;
	matrixResource_->operator[](0).wvp = worldMatrix_ * MyEngine::camera2D_->GetProjectionMatrix();
	
	// materialResource
	material_.lambertType = TYPE_NONE;
	material_.enableBlinnPhong = false;
}

void Sprite::EditorImGui(int identifier) {
	std::string title = "sprite##" + std::to_string(identifier);

	if (ImGui::TreeNode(title.c_str())) {

		// transform
		ImGui::DragFloat2("scale", &transform_.scale.x, 0.01f);
		ImGui::SliderAngle("rotate", &transform_.rotate.z);
		ImGui::DragFloat2("transform", &transform_.translate.x, 1.0f);

		// material
		if (ImGui::TreeNode("material")) {

			ImGui::ColorEdit4("color", &material_.color.x);
			ImGui::Spacing();
			ImGui::DragFloat2("scale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("rotate", &uvTransform_.rotate.z);
			ImGui::DragFloat2("translate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);

			material_.uvTransform = Matrix::MakeAffine(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);

			ImGui::TreePop();
		}

		if (ImGui::Button("delete")) {
			isDelete_ = true;
			ImGui::TreePop();
			return;
		}

		// 再計算
		worldMatrix_ = Matrix::MakeAffine(transform_.scale, transform_.rotate, transform_.translate);
		matrixResource_->operator[](0).world = worldMatrix_;
		matrixResource_->operator[](0).wvp = worldMatrix_ * MyEngine::camera2D_->GetProjectionMatrix();

		ImGui::TreePop();
	}
}

void Sprite::Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) {
	MyEngine::SetPipeLineState(TEXTURE);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);

	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, directionalLight->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, MyEngine::camera3D_->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(4, MyEngine::GetTextureHandleGPU("resources/uvChecker.png"));

	commandList->DrawIndexedInstanced(indexResource_->GetSize(), 1, 0, 0, 0);
}

void Sprite::Term() {
	vertexResource_.Release();
	indexResource_.Release();
}
