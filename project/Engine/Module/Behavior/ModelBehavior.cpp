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
	MaterialComponent::CreateBuffer();
}

void ModelBehavior::SystemAttributeImGui() {
	TransformBehavior::SystemAttributeImGui();

	if (ImGui::TreeNode("material")) {
		MaterialComponent::SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (!model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		model->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		model->DrawCall(i);
	}
}

void ModelBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (!model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		model->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		model->DrawCall(i);
	}
}

void ModelBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (!model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		model->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		model->DrawCall(i);
	}
}
