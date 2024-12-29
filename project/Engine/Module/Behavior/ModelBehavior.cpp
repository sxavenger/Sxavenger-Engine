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
	DrawModel(frame, kDefaultVS_AlbedoPS_Deferred, kDefaultMS_AlbedoPS_Deferred);
}

void ModelBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawModel(frame, kDefaultVS_AlbedoPS, kDefaultMS_AlbedoPS);
}

void ModelBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawModel(frame, kDefaultVS_AlbedoPS, kDefaultMS_AlbedoPS);
}

void ModelBehavior::DrawModel(const SxavGraphicsFrame* frame, RenderPipelineType vertex, RenderPipelineType mesh) {
	if (!model_.has_value()) {
		return;
	}

	model_.value().CheckAndReload();
	std::shared_ptr<Model> model = model_.value().Lock();

	if (!model->IsCompleted()) {
		return;
	}

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	parameter.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	parameter.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		if (model->GetInputMesh(i).IsCreateMeshlet()) {
			sConsole->SetGraphicsPipeline(mesh, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

			DxObject::BindBufferDesc bind = parameter;
			bind.Merge(model->GetInputMesh(i).GetMeshletBindBufferDesc());
			bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
			sConsole->BindGraphicsBuffer(mesh, SxavengerSystem::GetMainThreadContext(), bind);

			model->GetInputMesh(i).Dispatch(SxavengerSystem::GetMainThreadContext());

		} else {
			sConsole->SetGraphicsPipeline(vertex, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

			model->SetIABuffer(i);

			DxObject::BindBufferDesc bind = parameter;
			bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
			sConsole->BindGraphicsBuffer(vertex, SxavengerSystem::GetMainThreadContext(), bind);

			model->DrawCall(i);
		}
	}
}
