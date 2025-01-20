#include "AManualGeometryActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/!Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AManualGeometryActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AManualGeometryActor::Init() {
}

void AManualGeometryActor::RenderOpaque(const RendererContext& context) {
	if (!ia_.IsCreate()) {
		return;
	}

	auto texture = texture_.WaitGet();

	FRenderCore::GetInstance()->GetGeometry()->SetPipeline(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, context.size
	);

	ia_.BindIABuffer(context.context);

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",        TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gTextureComponent", TextureComponent::GetGPUVirtualAddress());
	parameter.SetHandle("gAlbedo",            texture->GetGPUHandleSRV());

	FRenderCore::GetInstance()->GetGeometry()->BindGraphicsBuffer(
		FRenderCoreGeometry::RenderType::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
		context.context, parameter
	);

	ia_.DrawCall(context.context);
}

void AManualGeometryActor::RenderTransparent(const RendererContext& context) {
	context;;
}

void AManualGeometryActor::SetupToplevelAS(const SetupContext& context) {

	DxrObject::TopLevelAS::Instance instance = {};
	instance.flag       = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	instance.mat        = TransformComponent::GetMatrix();
	instance.instanceId = 0;

	instance.expt = &FRenderCore::GetInstance()->GetRaytracing()->GetHitgroupExport(FRenderCoreRaytracing::HitgroupExportType::Geometry);

	DxrObject::WriteBindBufferDesc desc = {};
	desc.SetAddress(0, ia_.GetVertex()->GetGPUVirtualAddress()); //!< gVertices
	desc.SetAddress(1, ia_.GetIndex()->GetGPUVirtualAddress());  //!< gIndices
	desc.SetHandle(2, texture_.WaitGet()->GetGPUHandleSRV());    //!< gAlbedo

	instance.parameter = desc;

	ia_.CreateBottomLevelAS(context.context);
	instance.bottomLevelAS = ia_.GetBottomLevelAS();
}
