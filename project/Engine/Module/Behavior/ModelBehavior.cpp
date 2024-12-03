#include "ModelBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelBehavior::Init() {
	TransformComponent::CreateBuffer();

	mat_ = std::make_unique<DxObject::DimensionBuffer<Matrix4x4>>();
	mat_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*mat_)[0] = Matrix4x4::Identity();
}

void ModelBehavior::SystemAttributeImGui() {
	if (ImGui::TreeNode("Transform Component")) {
		TransformComponent::SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_ == nullptr) { return; }

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", mat_->GetGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		model_->DrawCall(i);
	}
}

void ModelBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_ == nullptr) { return; }

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", mat_->GetGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		model_->DrawCall(i);
	}
}

void ModelBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawAdaptive(frame);
}
