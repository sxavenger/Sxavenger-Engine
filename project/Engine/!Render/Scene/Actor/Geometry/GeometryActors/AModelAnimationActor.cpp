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

void AModelAnimationActor::SetupToplevelAS(const SetupContext& context) {
	auto model = model_.WaitGet();

	DxrObject::TopLevelAS::Instance instance = {};
	instance.flag       = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instance.mat        = TransformComponent::GetMatrix();
	instance.instanceId = 0;

	//instance.expt = &FRenderCore::GetInstance()->GetRaytracing()->GetHitgroupExport(FRenderCoreRaytracing::HitgroupExportType::Geometry);

	skeleton_->CreateBottomLevelAS(context.context);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		instance.bottomLevelAS = &skeleton_->GetBottomLevelAS(i);

		//DxrObject::WriteBindBufferDesc desc = {};
		//desc.SetAddress(0, model->GetInputMesh(i).GetVertex()->GetGPUVirtualAddress()); //!< gVertices
		//desc.SetAddress(1, model->GetInputMesh(i).GetIndex()->GetGPUVirtualAddress());  //!< gIndices
		//desc.SetHandle(2, model->GetTextureHandle(i));                                  //!< gAlbedo

		//instance.parameter = desc;

		context.toplevelAS->AddInstance(instance);
	}
}
