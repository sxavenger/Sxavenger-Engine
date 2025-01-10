#include "InstanceBehavior.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InstanceBehavior::Init() {
	MaterialComponent::CreateBuffer();
}

void InstanceBehavior::CreateInstance(uint32_t size) {
	matrixInstance_ = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	matrixInstance_->Create(SxavengerSystem::GetDxDevice(), size);
	std::generate(matrixInstance_->GetMappedData().begin(), matrixInstance_->GetMappedData().end(), []() { return TransformationMatrix{}; });
}

void InstanceBehavior::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawModel(frame, kDefaultVS_AlbedoPS_Deferred, kDefaultMS_AlbedoPS_Deferred);
}

void InstanceBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawModel(frame, kDefaultVS_AlbedoPS, kDefaultMS_AlbedoPS);
}

void InstanceBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	DrawModel(frame, kDefaultVS_AlbedoPS, kDefaultMS_AlbedoPS);
}

void InstanceBehavior::DrawModel(const SxavGraphicsFrame* frame, RenderPipelineType vertex, RenderPipelineType mesh) {
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
	parameter.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	parameter.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	parameter.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		if (model->GetInputMesh(i).IsCreateMeshlet()) {
			sConsole->SetGraphicsPipeline(mesh, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

			DxObject::BindBufferDesc bind = parameter;
			bind.Merge(model->GetInputMesh(i).GetMeshletBindBufferDesc());
			bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
			sConsole->BindGraphicsBuffer(mesh, SxavengerSystem::GetMainThreadContext(), bind);

			model->GetInputMesh(i).Dispatch(SxavengerSystem::GetMainThreadContext(), matrixInstance_->GetSize());

		} else {
			sConsole->SetGraphicsPipeline(vertex, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

			model->SetIABuffer(i);

			DxObject::BindBufferDesc bind = parameter;
			bind.SetHandle("gAlbedo", model->GetTextureHandle(i));
			sConsole->BindGraphicsBuffer(vertex, SxavengerSystem::GetMainThreadContext(), bind);

			model->DrawCall(i, matrixInstance_->GetSize());
		}
	}
}
