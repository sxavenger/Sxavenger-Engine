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
	if (model_ == nullptr) { return; }

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		model_->DrawCall(i, matrixInstance_->GetSize());
	}
}

void InstanceBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_ == nullptr) { return; }

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		model_->DrawCall(i, matrixInstance_->GetSize());
	}
}

void InstanceBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_ == nullptr) { return; }

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		model_->DrawCall(i, matrixInstance_->GetSize());
	}
}
