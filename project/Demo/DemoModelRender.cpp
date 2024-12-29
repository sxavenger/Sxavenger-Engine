#include "DemoModelRender.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoModelRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoModelRender::Init(const std::filesystem::path& filepath, bool isUseMeshShader) {

	model_ = SxavengerAsset::TryImport<Model>(filepath);
	model_.value().Lock()->SetUseMeshShader(isUseMeshShader);
	SxavengerAsset::PushTask(model_.value().Lock());

	//!< default settings
	
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);
	ModelBehavior::SetName("demo model behavior");

}

void DemoModelRender::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
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

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) { //!< hack: コードを短くする
		if (model->GetInputMesh(i).IsCreateMeshlet()) {
			if (model->CheckTextureWithMeshIndex(i, Model::TextureType::Bump)) {
				sConsole->SetGraphicsPipeline(kDefaultMS_AlbedoNormalPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

				DxObject::BindBufferDesc bind = parameter;
				bind.Merge(model->GetInputMesh(i).GetMeshletBindBufferDesc());
				bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
				bind.SetHandle("gNormal", model->GetTextureHandle(i, Model::TextureType::Bump));
				sConsole->BindGraphicsBuffer(kDefaultMS_AlbedoNormalPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

				model->GetInputMesh(i).Dispatch(SxavengerSystem::GetMainThreadContext());

			} else {
				sConsole->SetGraphicsPipeline(kDefaultMS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

				DxObject::BindBufferDesc bind = parameter;
				bind.Merge(model->GetInputMesh(i).GetMeshletBindBufferDesc());
				bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
				sConsole->BindGraphicsBuffer(kDefaultMS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

				model->GetInputMesh(i).Dispatch(SxavengerSystem::GetMainThreadContext());
			}
		} else {
			if (model->CheckTextureWithMeshIndex(i, Model::TextureType::Bump)) {
				sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoNormalPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

				model->SetIABuffer(i);

				DxObject::BindBufferDesc bind = parameter;
				bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
				bind.SetHandle("gNormal", model->GetTextureHandle(i, Model::TextureType::Bump));
				sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoNormalPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

				model->DrawCall(i);

			} else {
				sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

				model->SetIABuffer(i);

				DxObject::BindBufferDesc bind = parameter;
				bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
				sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

				model->DrawCall(i);
			}
		}
	}
}
