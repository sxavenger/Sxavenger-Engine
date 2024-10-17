#include "ModelInstanceBehavior.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include "BetaConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ModelInstanceBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelInstanceBehavior::Init() {
}

void ModelInstanceBehavior::Term() {
}

void ModelInstanceBehavior::SystemAttributeImGui() {
}

void ModelInstanceBehavior::SystemDraw() {
	if (model_ == nullptr) {
		return;
	}

	Assert(instanceBuffer_ != nullptr, "Not create instance buffer.");

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		// TODO: Mesh shader instance描画への対応

		//* vertex shader draw *//
		sBetaConsole->SetPipeline(BetaConsolePipelineType::kDefaultInstance_Diffuse);

		model_->GetMesh(i).SetBuffer();

		commandList->SetGraphicsRootConstantBufferView(0, sBetaConsole->GetDisplayCamera()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootShaderResourceView(1, instanceBuffer_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, model_->GetTextureHandle(i));

		model_->GetMesh(i).DrawCall(instanceBuffer_->GetIndexSize());
	}
}

void ModelInstanceBehavior::SystemDrawLocalMesh() {
}

void ModelInstanceBehavior::CreateInstance(uint32_t instanceSize) {
	instanceBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), instanceSize);

	for (uint32_t i = 0; i < instanceSize; ++i) {
		(*instanceBuffer_)[i].Init();
	}
}
