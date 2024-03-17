#include "Emitter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Particle.h>

// adapter
#include <ExecutionSpeed.h>
#include <Random.h>

#include <externals/imgui/imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter methods
////////////////////////////////////////////////////////////////////////////////////////////

void Emitter::Init() {
	transform_.scale     = unitVector;
	transform_.rotate    = origin;
	transform_.translate = origin;

	size_ = unitVector;

	count_ = 3;

	frequency_     = 0.5f; //!< 0.5秒ごとに発生
	frequencyTime_ = 0.0f;

	// draw
	model_ = std::make_unique<Model>("Resources/model", "area.obj");

	matrix_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	matrix_->operator[](0).world = Matrix4x4::MakeIdentity();
	matrix_->operator[](0).wvp   = Matrix4x4::MakeIdentity();

	material_ = std::make_unique<DxObject::BufferResource<Vector4f>>(MyEngine::GetDevicesObj(), 1);
	material_->operator[](0) = { 0.1f, 1.0f, 0.1f, 1.0f };
}

void Emitter::Update(Particle* particle) {

	frequencyTime_ += ExecutionSpeed::freamsParSec_;

	if (frequencyTime_ >= frequency_) {
		for (int i = 0; i < count_; ++i) {
			Vector3f randomRenge = { Random::Generate(-size_.x / 2.0f, size_.x / 2.0f), Random::Generate(-size_.y / 2.0f, size_.y / 2.0f), Random::Generate(-size_.z / 2.0f, size_.z / 2.0f) };

			// 発生処理
			Element element;
			element.transform = transform_;
			element.transform.translate = transform_.translate + randomRenge;
			element.velocity  = { Random::Generate(-1.0f, 1.0f), Random::Generate(-1.0f, 1.0f), Random::Generate(-1.0f, 1.0f) };
			element.color     = { Random::Generate(0.0f, 1.0f), Random::Generate(0.0f, 1.0f), Random::Generate(0.0f, 1.0f), 1.0f };
			element.lifeTime  = Random::Generate(1.0f, 3.0f);

			particle->SetElements(element);
		}

		// 過ぎた時間を加味して計算
		frequencyTime_ -= frequency_;
	}
}

void Emitter::Draw() {

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

void Emitter::Term() {

}

void Emitter::SetOnImGuiEditor() {
	if (ImGui::CollapsingHeader("emitter")) {
		ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);
		ImGui::DragFloat3("size", &size_.x, 0.1f, 0.0f, 1000.0f);
		ImGui::InputInt("count", &count_);
	}

	Matrix4x4 worldMatrix = Matrix::MakeAffine({ size_ * 0.5f }, { origin }, { transform_.translate });

	matrix_->operator[](0).world = worldMatrix;
	matrix_->operator[](0).wvp   = worldMatrix * MyEngine::camera3D_->GetViewProjectionMatrix();
}
