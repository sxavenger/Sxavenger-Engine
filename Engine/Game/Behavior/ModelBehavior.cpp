#include "ModelBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelBehavior::Init() {
	transform_.Init();
}

void ModelBehavior::Term() {
}

void ModelBehavior::SystemAttributeImGui() {
	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	// TODO: mesh shaderへの対応
	sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->GetMesh(i).BindIABuffer();

		commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
		commandList->SetGraphicsRootShaderResourceView(1, transform_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, model_->GetTextureHandle(i));

		model_->GetMesh(i).DrawCall();
	}
}