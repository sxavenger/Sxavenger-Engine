#include "AModelActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AModelActor::Init() {
	TransformComponent::CreateBuffer();
}

void AModelActor::RenderOpaque(const RendererContext& context) {
	FRenderCore::GetInstance()->SetPipeline(
		FRenderCore::RenderType::Deffered, FRenderCore::VertexStage::DefaultVS, FRenderCore::PixelStage::Albedo,
		context.context, context.size
	);

	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera",    context.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gTransform", TransformComponent::GetGPUVirtualAddress());

	// todo: mesh shader

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));

		FRenderCore::GetInstance()->BindGraphicsBuffer(
			FRenderCore::RenderType::Deffered, FRenderCore::VertexStage::DefaultVS, FRenderCore::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i);
	}
}

void AModelActor::RenderTransparent(const RendererContext& context) {
	context;
}
