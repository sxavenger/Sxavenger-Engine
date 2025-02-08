#include "AModelActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AModelActor::Init() {
	TransformComponent::CreateBuffer();
	TextureComponent::CreateBuffer();

	AActor::name_ = "model actor";
}

void AModelActor::RenderOpaque(const RendererContext& context) {
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
	if (!isRenderWait_ && !model_.Get()->IsComplete()) {
		return;
	}

	auto model = model_.WaitGet();

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());

	// todo: mesh shader
	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {

		model->SetIABuffer(context.context, i);

		DxObject::BindBufferDesc bind = parameter;
		bind.SetHandle("gAlbedo", model->GetTextureHandle(i));

		FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
			FRenderCoreGeometry::RenderType::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context.context, bind
		);

		model->DrawCall(context.context, i);
	}
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

	//instance.expt = &FRenderCore::GetInstance()->GetRaytracing()->GetHitgroupExport(FRenderCoreRaytracing::HitgroupExportType::Geometry);

	model->CreateBottomLevelAS(context.context);

	for (uint32_t i = 0; i < model->GetMeshSize(); ++i) {
		instance.bottomLevelAS = model->GetInputMesh(i).GetBottomLevelAS();

		//DxrObject::WriteBindBufferDesc desc = {};
		//desc.SetAddress(0, model->GetInputMesh(i).GetVertex()->GetGPUVirtualAddress()); //!< gVertices
		//desc.SetAddress(1, model->GetInputMesh(i).GetIndex()->GetGPUVirtualAddress());  //!< gIndices
		//desc.SetHandle(2, model->GetTextureHandle(i));                                  //!< gAlbedo

		//instance.parameter = desc;

		context.toplevelAS->AddInstance(instance);
	}

}
