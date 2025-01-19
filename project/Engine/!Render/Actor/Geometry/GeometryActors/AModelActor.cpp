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
	TextureComponent::CreateBuffer();
}

void AModelActor::RenderOpaque(const RendererContext& context) {
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());

	// todo: mesh shader

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i);
	}
}

void AModelActor::RenderTransparent(const RendererContext& context) {
	context;
}

void AModelActor::SetupToplevelAS(const SetupContext& context) {

	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxrObject::TopLevelAS::Instance instance = {};
	instance.flag       = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instance.mat        = TransformComponent::GetMatrix();
	instance.instanceId = 0;

	instance.expt      = nullptr; //!< TODO: export group
	instance.parameter = nullptr; //!< TODO: bind buffer desc

	model->CreateBottomLevelAS(context.context);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		instance.bottomLevelAS = model->GetInputMesh(i).GetBottomLevelAS();
		context.toplevelAS->AddInstance(instance);
	}

}
