#include "AModelInstanceActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelInstanceActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AModelInstanceActor::Init(uint32_t instanceCount) {
	mat_ = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
	mat_->Create(SxavengerSystem::GetDxDevice(), instanceCount);

	TextureComponent::CreateBuffer();
}

void AModelInstanceActor::RenderOpaque(const RendererContext& context) {
	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        mat_->GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());

	// todo: mesh shader
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i, mat_->GetSize());
	}
		
}

void AModelInstanceActor::RenderTransparent(const RendererContext& context) {
	context;
}

void AModelInstanceActor::SetupToplevelAS(const SetupContext& context) {
	context;
}
