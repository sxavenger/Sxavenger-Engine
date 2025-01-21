#include "AModelAnimationActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelAnimationActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AModelAnimationActor::Init() {
	TransformComponent::CreateBuffer();
	TextureComponent::CreateBuffer();

	auto model = model_.WaitGet();
	skeleton_ = std::make_unique<SkeletonMesh>();
	skeleton_->Create(model);

	AActor::name_ = "model animation actor";
}

void AModelAnimationActor::RenderOpaque(const RendererContext& context) {
	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());

	// todo: mesh shader
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {

		skeleton_->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		skeleton_->DrawCall(context.context, i);
	}

}

void AModelAnimationActor::RenderTransparent(const RendererContext& context) {
	context;
}
