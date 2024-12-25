#include "AnimationBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationBehavior::SystemAttributeImGui() {
	ModelBehavior::SystemAttributeImGui();
}

void AnimationBehavior::DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	/*if (skeletonMesh_ == nullptr) {
		ModelBehavior::DrawSystematic(frame);
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skeletonMesh_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS_Deferred, SxavengerSystem::GetMainThreadContext(), bind);

		skeletonMesh_->DrawCall(i);
	}*/
}

void AnimationBehavior::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	/*if (skeletonMesh_ == nullptr) {
		ModelBehavior::DrawAdaptive(frame);
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skeletonMesh_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		skeletonMesh_->DrawCall(i);
	}*/
}

void AnimationBehavior::DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	/*if (skeletonMesh_ == nullptr) {
		ModelBehavior::DrawLateAdaptive(frame);
		return;
	}

	sConsole->SetGraphicsPipeline(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   TransformComponent::GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       MaterialComponent::GetColorGPUVirtualAddress());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skeletonMesh_->SetIABuffer(i);

		bind.SetHandle("gAlbedo", model_->GetTextureHandle(i));
		sConsole->BindGraphicsBuffer(kDefaultVS_AlbedoPS, SxavengerSystem::GetMainThreadContext(), bind);

		skeletonMesh_->DrawCall(i);
	}*/
}
