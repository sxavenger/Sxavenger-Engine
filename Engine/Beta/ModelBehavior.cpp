#include "ModelBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include "BetaConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelBehavior::Init() {
	transform_.Init();
}

void ModelBehavior::Term() {
}

void ModelBehavior::SystemAttributeImGui() {
	if (model_ != nullptr) {
		if (ImGui::TreeNode("mesh")) {
			sBetaConsole->DisplayLocalRenderTarget(1);
			ImGui::TreePop();
		}
	}

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::SystemDraw() {
	if (model_ == nullptr) {
		return;
	}

	sBetaConsole->SetPipeline(BetaConsolePipelineType::kDefault_Diffuse);

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->GetMesh(i).SetBuffer();

		commandList->SetGraphicsRootConstantBufferView(0, sBetaConsole->GetDisplayCamera()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, transform_.GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, model_->GetTextureHandle(i));

		model_->GetMesh(i).DrawCall();
	}
}

void ModelBehavior::SystemDrawLocalMesh() {
	if (model_ == nullptr) {
		return;
	}

	sBetaConsole->SetPipeline(BetaConsolePipelineType::kConsoleLocal);

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->GetMesh(i).SetBuffer();

		commandList->SetGraphicsRootConstantBufferView(0, sBetaConsole->GetLocalCamera()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(1, model_->GetTextureHandle(i));

		model_->GetMesh(i).DrawCall();
	}
}
