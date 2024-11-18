#include "ModelInstanceBehavior.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelInstanceBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelInstanceBehavior::Init() {
}

void ModelInstanceBehavior::Term() {
}

void ModelInstanceBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		if (model_->GetMesh(i).IsCreateMeshlet()) {

			sSystemConsole->SetRenderingPipeline(kDefaultMS_AlbedoPS_Deferred);
			//!< meshlet生成がされているのでmesh shaderで描画

			commandList->SetGraphicsRootConstantBufferView(5, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(7, instanceBuffer_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(8, model_->GetTextureHandle(i));

			model_->GetMesh(i).Dispatch(0, 1, 2, 3, 4, 6, instanceBuffer_->GetIndexSize());

		} else {

			sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);
			//!< vertex shaderで描画

			model_->GetMesh(i).BindIABuffer();

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, instanceBuffer_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));

			model_->GetMesh(i).DrawCall(instanceBuffer_->GetIndexSize());
		}
	}

}

void ModelInstanceBehavior::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		if (model_->GetMesh(i).IsCreateMeshlet()) {

			sSystemConsole->SetRenderingPipeline(kDefaultMS_AlbedoPS);
			//!< meshlet生成がされているのでmesh shaderで描画

			commandList->SetGraphicsRootConstantBufferView(5, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(7, instanceBuffer_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(8, model_->GetTextureHandle(i));

			model_->GetMesh(i).Dispatch(0, 1, 2, 3, 4, 6, instanceBuffer_->GetIndexSize());

		} else {

			sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS);
			//!< vertex shaderで描画

			model_->GetMesh(i).BindIABuffer();

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, instanceBuffer_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));

			model_->GetMesh(i).DrawCall(instanceBuffer_->GetIndexSize());
		}
	}
}

void ModelInstanceBehavior::CreateInstance(uint32_t instanceSize) {
	instanceBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), instanceSize);
}
