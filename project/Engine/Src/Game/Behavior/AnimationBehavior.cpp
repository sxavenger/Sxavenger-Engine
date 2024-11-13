#include "AnimationBehavior.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationBehavior::Init() {
}

void AnimationBehavior::Term() {
}

void AnimationBehavior::CreateAnimation() {
	Assert(model_ != nullptr, "model is not set.");
	animator_ = std::make_unique<Animator>();
	animator_->Create(model_);
}

void AnimationBehavior::UpdateAnimator(uint32_t animaitonIndex, bool isLoop) {
	animator_->Update(animationTime_, animaitonIndex, isLoop);
}

void AnimationBehavior::SystemAttributeImGui() {
	ModelBehavior::SystemAttributeImGui();

	ImGui::DragFloat("animation time", &animationTime_.time, 0.01f);
}

void AnimationBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	if (animator_ == nullptr) {
		ModelBehavior::DrawSystematic(camera); //!< animatorが設定されていないので通常描画

	} else {
		//!< animatorの描画
		
		auto commandList = Sxavenger::GetCommandList();

		// TODO: mesh shaderへの対応
		sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);

		for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
			// animator適用のIAを設定
			animator_->BindIABuffer(i);

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, transform_.GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));

			model_->GetMesh(i).DrawCall();
		}
	}
}
