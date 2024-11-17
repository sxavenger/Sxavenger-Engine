#include "ModelBehavior.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelBehavior::Init() {
	transform_.Create();
	uvTransform_.Create();
	material_.Create();
	color_.Create();
}

void ModelBehavior::Term() {
}

void ModelBehavior::CreateRaytracingRecorder() {
	recorders_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		recorders_[i] = std::make_unique<BufferRecoreder>();
		recorders_[i]->Create(sSystemConsole->GetRaytracingPipeline()->GetExport(kHitgroup_Behavior, 0));

		recorders_[i]->SetAddress(0, model_->GetMesh(i).GetVertexBuffer()->GetGPUVirtualAddress()); //!< Vertices
		recorders_[i]->SetAddress(1, model_->GetMesh(i).GetIndexBuffer()->GetGPUVirtualAddress());  //!< Indices
		recorders_[i]->SetAddress(2, uvTransform_.GetVirtualAddress());                             //!< UVTransform
		recorders_[i]->SetHandle(3, model_->GetTextureHandle(i));                                   //!< Albedo
		recorders_[i]->SetAddress(4, material_.GetGPUVirtualAddress());                             //!< PBRMaterial
	}
}

void ModelBehavior::SystemAttributeImGui() {
	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		material_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("UV transform")) {
		uvTransform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("color")) {
		color_.SetImGuiCommand();
		ImGui::TreePop();
	}
}

void ModelBehavior::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		if (model_->GetMesh(i).IsCreateMeshlet()) {

			sSystemConsole->SetRenderingPipeline(kDefaultMS_AlbedoPS_Deferred);
			//!< meshlet生成がされているのでmesh shaderで描画

			commandList->SetGraphicsRootConstantBufferView(5, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(7, transform_.GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(8, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(9, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(10, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).Dispatch(0, 1, 2, 3, 4, 6);

		} else {
			
			sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);
			//!< vertex shaderで描画

			model_->GetMesh(i).BindIABuffer();

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, transform_.GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(4, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).DrawCall();
		}
	}
}

void ModelBehavior::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {

	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		if (model_->GetMesh(i).IsCreateMeshlet()) {

			sSystemConsole->SetRenderingPipeline(kDefaultMS_AlbedoPS);
			//!< meshlet生成がされているのでmesh shaderで描画

			commandList->SetGraphicsRootConstantBufferView(5, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(7, transform_.GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(8, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(9, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(10, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).Dispatch(0, 1, 2, 3, 4, 6);

		} else {
			
			sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS);
			//!< vertex shaderで描画

			model_->GetMesh(i).BindIABuffer();

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, transform_.GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(4, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).DrawCall();
		}
	}
}

void ModelBehavior::DrawRaytracing(_MAYBE_UNUSED DxrObject::TopLevelAS* tlas) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	Matrix4x4 mat = transform_.GetWorldMatrix();

	Assert(model_->GetMeshSize() <= recorders_.size(), "raytracing recorder not created.");

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		tlas->SetInstance(model_->GetMesh(i).GetBLAS(), mat, recorders_.at(i).get(), 0);
	}
}
