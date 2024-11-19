#include "Teapot.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	SetName("teapot");

	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "bunny.obj");
	//model_->ApplyRaytracing();
	//CreateRaytracingRecorder();

	model_->ApplyMeshShader();

	renderingFlag_ = /*kBehaviorRender_Raytracing |*/ kBehaviorRender_Systematic;

	matrix_ = std::make_unique<DxObject::BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), kIntanceCount_);

	for (uint32_t i = 0; i < kIntanceCount_; ++i) {
		QuaternionTransform transform = {};
		transform.translate = { 0.0f, 0.0f, static_cast<float>(i) };

		(*matrix_)[i].Transfer(transform.ToMatrix());
	}
}

void Teapot::SetAttributeImGui() {
	ImGui::DragFloat2("pos", &pos_.x, 0.01f);
	ImGui::DragFloat2("size", &size_.x, 0.01f);
	ImGui::DragFloat("rotate", &rotate_, 0.01f);
}

void Teapot::DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {
	if (model_ == nullptr) {
		return; //!< modelが設定されていない
	}

	auto commandList = Sxavenger::GetCommandList();

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		if (model_->GetMesh(i).IsCreateMeshlet()) {

			sSystemConsole->SetRenderingPipeline(kDefaultMS_AlbedoPS_Deferred);
			//!< meshlet生成がされているのでmesh shaderで描画

			commandList->SetGraphicsRootConstantBufferView(5, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(7, matrix_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(8, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(9, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(10, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).Dispatch(0, 1, 2, 3, 4, 6, matrix_->GetIndexSize());

		} else {
			
			sSystemConsole->SetRenderingPipeline(kDefaultVS_AlbedoPS_Deferred);
			//!< vertex shaderで描画

			model_->GetMesh(i).BindIABuffer();

			commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(1, matrix_->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(2, uvTransform_.GetVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(3, model_->GetTextureHandle(i));
			commandList->SetGraphicsRootConstantBufferView(4, color_.GetGPUVirtualAddress());

			model_->GetMesh(i).DrawCall();
		}
	}
}

void Teapot::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	SxavengerGame::DrawSprite(
		pos_, size_, rotate_, Sxavenger::GetTextureHandleGPU("resources/uvChecker.png"), {1.0f, 1.0f, 1.0f, 1.0f}
	);
}
