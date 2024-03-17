#include "Field.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Particle.h>
#include <ExecutionSpeed.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Field methods
////////////////////////////////////////////////////////////////////////////////////////////

void Field::Init() {
	pos_ = { 0.0f, 0.0f, 0.0f };
	size_ = { 2.0f, 2.0f, 2.0f };
	acceleration_ = { -15.0f, 0.0f, 0.0f };

	model_ = std::make_unique<Model>("Resources/model", "area.obj");

	matrix_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	matrix_->operator[](0).world = Matrix4x4::MakeIdentity();
	matrix_->operator[](0).wvp   = Matrix4x4::MakeIdentity();

	material_ = std::make_unique<DxObject::BufferResource<Vector4f>>(MyEngine::GetDevicesObj(), 1);
	material_->operator[](0) = { 1.0f, 1.0f, 0.0f, 1.0f };
}

void Field::Update(Particle* particle) {

	AABB aabb;
	aabb.min = pos_ - (size_ * 0.5f);
	aabb.max = pos_ + (size_ * 0.5f);

	for (auto& it : particle->GetList()) {
		if (Collider::AABBToPoint(aabb, it.transform.translate)) {
			it.velocity += acceleration_ * ExecutionSpeed::freamsParSec_;
		}
	}

}

void Field::Draw() {

	MyEngine::SetBlendMode(BlendMode::kBlendModeNormal);
	MyEngine::SetPipelineType(DxObject::AREA);
	MyEngine::SetPipelineState();

	for (uint32_t i = 0; i < model_->GetSize(); ++i) {
		model_->SetBuffers(MyEngine::GetCommandList(), i);

		MyEngine::GetCommandList()->SetGraphicsRootConstantBufferView(0, matrix_->GetGPUVirtualAddress());
		MyEngine::GetCommandList()->SetGraphicsRootConstantBufferView(1, material_->GetGPUVirtualAddress());
		model_->SetTexture(2, MyEngine::GetCommandList(), i);

		model_->DrawCall(MyEngine::GetCommandList(), i, 1);
	}
}

void Field::Term() {
	model_.reset();
}

void Field::SetOnImGuiEditor() {
	if (ImGui::CollapsingHeader("field")) {
		ImGui::DragFloat3("pos", &pos_.x, 0.1f);
		ImGui::DragFloat3("size", &size_.x, 0.01f, 0.0f, 1000.0f);
		ImGui::DragFloat3("acceleration", &acceleration_.x, 0.1f);
	}

	Matrix4x4 worldMatrix = Matrix::MakeAffine({size_ * 0.5f}, {origin}, {pos_});

	matrix_->operator[](0).world = worldMatrix;
	matrix_->operator[](0).wvp   = worldMatrix * MyEngine::camera3D_->GetViewProjectionMatrix();

}
