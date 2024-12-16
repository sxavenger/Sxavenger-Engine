#include "Illumination.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Content/SxavengerContent.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Illmaination class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Illmination::Init() {
	SetName("illmination tree");

	model_ = SxavengerAsset::ImportModel("asset/model/tree/tree.obj");
	model_->AsyncLoad();

	SxavengerSystem::PushTask(model_);
	model_->WaitCompleted();

	InstanceBehavior::model_         = model_.get();
	InstanceBehavior::SetRenderingFlag(BehaviorRenderingType::kAdaptive);
	InstanceBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);

	CreateInstance(kInstanceCount_);

	for (uint32_t i = 0; i < kInstanceCount_; ++i) {

		QuaternionTransform q = {};
		q.scale     = { 0.8f, 0.8f, 0.8f };
		q.rotate    = MakeAxisAngle({0.0f, 1.0f, 0.0f}, Random::Generate(0.0f, pi_v * 2.0f));
		q.translate = { Random::Generate(2.0f, 3.0f), 0.0f, Random::Generate(2.0f, 3.0f) * i - (2.0f * kInstanceCount_  / 2)};

		if (i % 2 == 0) {
			q.translate.x *= -1;
		}

		GetMatrix(i).Transfer(q.ToMatrix());
	}

	//* shape

	material_.CreateBuffer();
	material_.GetColor() = ToColor4f(0x9E9D6EB0);
	material_.Transfer();

	materialPoint_.CreateBuffer();
	materialPoint_.GetColor() = ToColor4f(0xC6C313FF);
	materialPoint_.Transfer();

	stateShape_ = std::make_unique<ReflectionGraphicsPipelineState>();
	stateShape_->CreateBlob("demo/shape.vs.hlsl",                GraphicsShaderType::vs);
	stateShape_->CreateBlob("demo/shape.gs.hlsl",                GraphicsShaderType::gs);
	stateShape_->CreateBlob("sxavenger/behavior/albedo.ps.hlsl", GraphicsShaderType::ps);

	stateShape_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.SetBlendMode(0, BlendMode::kBlendModeAdd);

	stateShape_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	statePoint_ = std::make_unique<ReflectionGraphicsPipelineState>();
	statePoint_->CreateBlob("demo/point.vs.hlsl",                GraphicsShaderType::vs);
	statePoint_->CreateBlob("demo/point.gs.hlsl",                GraphicsShaderType::gs);
	statePoint_->CreateBlob("sxavenger/behavior/albedo.ps.hlsl", GraphicsShaderType::ps);

	statePoint_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	desc.primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	desc.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	statePoint_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	dof_ = std::make_unique<VisualDoF>();
	dof_->Init();
	dof_->SetToConsole("illmination dof");
	dof_->GetParameter().focusLength = 24.0f;
	dof_->GetParameter().f           = 8.0f;
}

void Illmination::SetAttributeImGui() {
	if (ImGui::TreeNode("illmination material")) {
		material_.SetImGuiCommand();
		ImGui::TreePop();
	}
}

void Illmination::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {
	if (model_ == nullptr) { return; }

	auto command = SxavengerSystem::GetMainThreadContext()->GetDxCommand();

	stateShape_->ReloadAndSetPipeline(command);

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       material_.GetColorGPUVirtualAddress());
	bind.SetHandle("gAlbedo",       SxavengerContent::GetTextureGPUHandleSRV("white1x1"));
	stateShape_->BindGraphicsBuffer(command, bind);


	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);
		model_->DrawCall(i, kInstanceCount_);
	}

	statePoint_->ReloadAndSetPipeline(command);

	bind.SetAddress("gColor", materialPoint_.GetColorGPUVirtualAddress());

	statePoint_->BindGraphicsBuffer(command, bind);

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);
		model_->DrawCall(i, kInstanceCount_);
	}
}
