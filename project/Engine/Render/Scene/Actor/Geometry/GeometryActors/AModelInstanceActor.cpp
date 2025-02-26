#include "AModelInstanceActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelInstanceActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AModelInstanceActor::Init(uint32_t instanceCount) {
	Transform2dComponent::CreateBuffer();

	mat_ = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
	mat_->Create(SxavengerSystem::GetDxDevice(), instanceCount);
	mat_->Fill({});

	SetRenderInstanceCount(instanceCount);
}

void AModelInstanceActor::Render(const RendererContext& context) {
	if (!isRenderWait_ && !model_.Get()->IsComplete()) { //!< モデルが読み込み中の場合
		return;
	}

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",   mat_->GetGPUVirtualAddress());
	parameter.SetAddress("gTransform2d", Transform2dComponent::GetGPUVirtualAddress());

	if (context.target == MaterialComponent::BlendMode::Opaque) {
		RenderOpaque(context, parameter);

	} else if (context.target == MaterialComponent::BlendMode::Translucent) {
		RenderTranslucent(context, parameter);
	}
}

void AModelInstanceActor::SetupToplevelAS(const SetupContext& context) {
	context;
}

void AModelInstanceActor::SetRenderInstanceCount(std::optional<uint32_t> count) {
	Assert(count.value_or(mat_->GetSize()) <= mat_->GetSize(), "render instance count is max instance over.");
	renderInstanceCount_ = count.value_or(mat_->GetSize());
}

void AModelInstanceActor::RenderOpaque(const RendererContext& context, const DxObject::BindBufferDesc& parameter) {

	auto model = model_.WaitGet();

	// todo: mesh shader
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		if (model->GetMeshMaterial(i).GetBlendMode() != MaterialComponent::BlendMode::Opaque) {
			continue;
		}

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetAddress("gMaterial", model->GetMeshMaterial(i).GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i, renderInstanceCount_);
	}
}

void AModelInstanceActor::RenderTranslucent(const RendererContext& context, const DxObject::BindBufferDesc& parameter) {

	auto model = model_.WaitGet();

	// todo: mesh shader
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		if (model->GetMeshMaterial(i).GetBlendMode() == MaterialComponent::BlendMode::Opaque) {
			continue;
		}

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetAddress("gMaterial", model->GetMeshMaterial(i).GetGPUVirtualAddress());

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i, renderInstanceCount_);
	}

}
