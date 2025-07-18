#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"
#include "../FMainRender.h"

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXThreadContext* context, const Config& config) {

	//* configの確認
	Config conf = config;
	ApplyConfig(conf);

	Sxl::Flag<Status> status = CheckStatus(conf);

	// texturesの確認
	if (!status.Test(Status::Error_Textures)) {
		//textures_->ClearTextures(context);
	}

	if (status.Any(Status::Status_Error)) {
		return;
	}

	switch (config.technique) {
		case GraphicsTechnique::Deferred:
			textures_->ClearTextures(context);
			RenderTechniqueDeferred(context, conf);
			break;

		/*case GraphicsTechnique::Raytracing:
			RenderTechniqueRaytracing(context, conf);
			break;*/

		case GraphicsTechnique::Pathtracing:
			//textures_->ClearTexturesPathtracing(context);
			RenderTechniquePathtracing(context, conf);
			break;
	}

	context->TransitionAllocator();
}

void FSceneRenderer::ResetReserviour(const DirectXThreadContext* context) {
	if (reservoir_ != nullptr) {
		reservoir_->At(0).ResetFrame(); //!< reservoirのリセット
	}

	if (textures_ != nullptr) {
		textures_->ClearTextures(context); //!< texturesのリセット
	}

	isResetReservoir_ = true;
}

const uint32_t FSceneRenderer::GetCurrentSampleCount() const {
	if (reservoir_ == nullptr) {
		return 0;
	}

	return reservoir_->At(0).GetCurrentSampleCount();
}

void FSceneRenderer::ApplyConfig(Config& config) {
	if (config.camera == nullptr) { //!< cameraが設定されていない場合, Tagのcameraを取得
		if (config.tag != CameraComponent::Tag::None) {

			// component storage から tag に一致する camera component を取得.
			sComponentStorage->ForEach<CameraComponent>([&](CameraComponent* component) {
				if (component->GetTag() == config.tag) {
					config.camera = component;
				}
			});
		}
	}

	if (config.scene == nullptr) { //!< sceneが設定されていない場合, main renderから取得
		config.scene = FMainRender::GetInstance()->GetScene();
	}
}

Sxl::Flag<FSceneRenderer::Status> FSceneRenderer::CheckStatus(const Config& config) const {

	Sxl::Flag<Status> status = Status::Success;

	if (textures_ == nullptr) {
		Logger::WarningRuntime("warning | [FSceneRenderer]::Status", "textures is nullptr.");
		status |= Status::Error_Textures;
	}

	if (config.camera == nullptr) {
		Logger::WarningRuntime("warning | [FSceneRenderer]::Status", "camera is nullptr.");
		status |= Status::Error_Camera;
	}

	return status;
}

void FSceneRenderer::RenderGeometryPass(const DirectXThreadContext* context, const Config& config) {
	textures_->BeginGeometryPass(context);

	RenderGeometryStaticMeshDefault(context, config);
	RenderGeometrySkinnedMesh(context, config);

	textures_->EndGeometryPass(context);
}

void FSceneRenderer::RenderGeometryStaticMeshDefault(const DirectXThreadContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		component->GetMesh()->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->GetMesh()->DrawCall(context, 1);

	});

	//context->TransitionAllocator();
}

void FSceneRenderer::RenderGeometrySkinnedMesh(const DirectXThreadContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		
		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			// 透明なジオメトリは別のパスで描画
			return;
		}
		component->BindIABuffer(context);

		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->DrawCall(context, 1);
	});
}

void FSceneRenderer::LightingPass(const DirectXThreadContext* context, const Config& config) {

	textures_->BeginLightingPass(context);

	//!< EmptyLight
	LightingEmpty(context, config);

	//!< DirectionalLight
	LightingPassDirectionalLight(context, config);

	//!< PointLight
	LightingPassPointLight(context, config);

	//!< RectLight
	LightingPassRectLight(context, config);

	//!< SkyLight
	LightingPassSkyLight(context, config);

	textures_->EndLightingPass(context);

}

void FSceneRenderer::LightingEmpty(const DirectXThreadContext* context, const Config& config) {
	FRenderCoreLight::LightType emptyType = FRenderCoreLight::LightType::Empty;

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		emptyType, context, textures_->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleSRV());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		emptyType, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context);
}

void FSceneRenderer::LightingPassDirectionalLight(const DirectXThreadContext* context, const Config& config) {

	uint32_t count = config.scene->directionalLightCount_->At(0);

	if (count == 0) {
		return;
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Directional, context, textures_->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleSRV());

	// direcitonal light parameter
	parameter.SetAddress("gTransforms", config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	parameter.SetAddress("gParameters", config.scene->directionalLightParams_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadows",    config.scene->directionalLightShadowParams_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Directional, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context, count);
}

void FSceneRenderer::LightingPassPointLight(const DirectXThreadContext* context, const Config& config) {

	uint32_t count = config.scene->pointLightCount_->At(0);

	if (count == 0) {
		return;
	}

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Point, context, textures_->GetSize()
	);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	// deferred paraemter
	parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
	parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
	parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
	parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleSRV());

	// point light parameter
	parameter.SetAddress("gTransforms", config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	parameter.SetAddress("gParameters", config.scene->pointLightParams_->GetGPUVirtualAddress());
	parameter.SetAddress("gShadows",    config.scene->pointLightShadowParams_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
		FRenderCoreLight::LightType::Point, context, parameter
	);

	FRenderCore::GetInstance()->GetLight()->DrawCall(context, count);

}

void FSceneRenderer::LightingPassRectLight(const DirectXThreadContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Rect, context, textures_->GetSize()
	);

	sComponentStorage->ForEachActive<RectLightComponent>([&](RectLightComponent* component) {

		DxObject::BindBufferDesc parameter = {};
		// common parameter
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

		// deferred paraemter
		parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
		parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleSRV());

		// point light parameter
		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameter",  component->GetParameterBufferAddress());
		parameter.SetAddress("gShadow",     component->GetShadowBufferAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::Rect, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});

}

void FSceneRenderer::LightingPassSkyLight(const DirectXThreadContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLight, context, textures_->GetSize()
	);

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {

		DxObject::BindBufferDesc parameter = {};
		// common parameter
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

		// deferred paraemter
		parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
		parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleSRV());

		// sky light parameter
		parameter.SetAddress("gDiffuseParameter",  component->GetDiffuseParameterBufferAddress());
		parameter.SetAddress("gSpecularParameter", component->GetSpecularParameterBufferAddress());
		parameter.SetAddress("gParameter",         component->GetParameterBufferAddress());
		parameter.SetHandle("gBRDFLut",            FRenderCore::GetInstance()->GetBRDFLut());

		parameter.SetHandle("gEnvironment",        component->GetEnvironment().value()); //!< DEBUG

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyLight, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});
}

void FSceneRenderer::AmbientProcessPass(const DirectXThreadContext* context, const Config& config) {
	AmbientProcessPassSkyBox(context, config);
}

void FSceneRenderer::AmbientProcessPassSkyBox(const DirectXThreadContext* context, const Config& config) {
	// 関数名を変更するかも

	textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->TransitionBeginUnordered(context);

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::ProcessType::Environment, context
	);

	sComponentStorage->ForEachActive<SkyLightComponent>([&](SkyLightComponent* component) {
		if (!component->GetEnvironment().has_value()) {
			return;
		}

		DxObject::BindBufferDesc parameter = {};
		// common
		parameter.SetAddress("gConfig", textures_->GetDimension());
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

		// deferred
		parameter.SetHandle("gDepth", textures_->GetDepth()->GetRasterizerGPUHandleSRV());

		// environment
		parameter.SetAddress("gParameter", component->GetParameterBufferAddress());
		parameter.SetHandle("gEnvironment", (*component->GetEnvironment()));

		// output
		parameter.SetHandle("gOutput", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleUAV());

		FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
			FRenderCoreProcess::ProcessType::Environment, context, parameter
		);

		FRenderCore::GetInstance()->GetProcess()->Dispatch(context, textures_->GetSize());
	});

	textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->TransitionEndUnordered(context);
}

void FSceneRenderer::RenderTransparentBasePass(const DirectXThreadContext* context, const Config& config) {

	textures_->BeginTransparentBasePass(context);

	RenderTransparentBaseStaticMesh(context, config);
	RenderTransparentParticle(context, config);

	textures_->EndTransparentBasePass(context);
}

void FSceneRenderer::RenderTransparentBaseStaticMesh(const DirectXThreadContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	// componentを取得
	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		if (!component->IsView()) {
			return;
		}

		if (component->GetMaterial()->GetMode() != Material::Mode::Translucent) {
			return;
		}

		component->GetMesh()->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  component->GetMaterial()->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Forward, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		component->GetMesh()->DrawCall(context, 1);

	});
}

void FSceneRenderer::RenderTransparentParticle(const DirectXThreadContext* context, const Config& config) {

	// componentを取得
	sComponentStorage->ForEachActive<ParticleComponent>([&](ParticleComponent* component) {
		component->DrawParticle(context, config.camera);
	});
}

void FSceneRenderer::PostProcessPass(const DirectXThreadContext* context, const Config& config) {
	if (!config.isEnablePostProcess) {
		return;
	}

	textures_->BeginPostProcess(context);

	// componentを取得
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Global) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		component->Process(context, textures_, config.camera);
	});
	

	textures_->EndPostProcess(context);
}

void FSceneRenderer::CompositeProcessPass(const DirectXThreadContext* context, const Config& config) {

	if (!config.isEnableComposite) {
		return;
	}

	textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->TransitionBeginUnordered(context);

	CompositeProcessPassTonemap(context, config);

	// componentを取得
	sComponentStorage->ForEachActive<CompositeProcessLayerComponent>([&](CompositeProcessLayerComponent* component) {
		component->Process(context, textures_);
	});
	//!< HACK: componentのtagなどを作成.

	textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->TransitionEndUnordered(context);

	{ //!< barrier
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetResource();
		context->GetCommandList()->ResourceBarrier(1, &barrier);
	}

}

void FSceneRenderer::CompositeProcessPassTonemap(const DirectXThreadContext* context, const Config& config) {

	config;

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::CompositeType::Tonemap, context
	);

	DxObject::BindBufferDesc parameter = {};
	// common
	parameter.SetAddress("gConfig", textures_->GetDimension());
	parameter.SetHandle("gTexture", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleUAV());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
		FRenderCoreProcess::CompositeType::Tonemap, context, parameter
	);

	FRenderCore::GetInstance()->GetProcess()->Dispatch(context, textures_->GetSize());

}

void FSceneRenderer::RenderTechniqueDeferred(const DirectXThreadContext* context, const Config& config) {

	RenderGeometryPass(context, config);

	LightingPass(context, config);

	AmbientProcessPass(context, config);

	RenderTransparentBasePass(context, config);

	PostProcessPass(context, config);

	CompositeProcessPass(context, config);
}

void FSceneRenderer::RenderTechniqueRaytracing(const DirectXThreadContext* context, const Config& config) {
	// preview機能

	textures_->BeginRaytracingPass(context);

	config.scene->GetStateObjectContext().SetStateObject(context->GetDxCommand());

	auto commandList = context->GetCommandList();
	commandList->SetComputeRootDescriptorTable(0, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(1, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(2, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(3, textures_->GetDepth()->GetRaytracingGPUHandleUAV());
	commandList->SetComputeRootConstantBufferView(4, config.camera->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(5, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	commandList->SetComputeRootConstantBufferView(6, config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(7, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(8, config.scene->directionalLightParams_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(9, config.scene->directionalLightShadowParams_->GetGPUVirtualAddress());

	config.scene->GetStateObjectContext().DispatchRays(context->GetDxCommand(), textures_->GetSize());

	textures_->EndRaytracingPass(context);
}

void FSceneRenderer::RenderTechniquePathtracing(const DirectXThreadContext* context, const Config& config) {
	// preview機能

	if (reservoir_ == nullptr) {
		reservoir_ = std::make_unique<DxObject::DimensionBuffer<FRenderCorePathtracing::Reservoir>>();
		reservoir_->Create(SxavengerSystem::GetDxDevice(), 1);
		reservoir_->At(0).Init();

	} else {
		reservoir_->At(0).IncrimentFrame();

		if (isResetReservoir_) {
			reservoir_->At(0).ResetFrame(); //!< reservoirのリセット
			isResetReservoir_ = false;
		}
	}

	textures_->BeginRaytracingPass(context);

	config.scene->GetStateObjectContext().SetStateObject(context->GetDxCommand());

	auto commandList = context->GetCommandList();
	commandList->SetComputeRootDescriptorTable(0, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(1, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(2, textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::MaterialARM)->GetGPUHandleUAV());
	commandList->SetComputeRootDescriptorTable(3, textures_->GetDepth()->GetRaytracingGPUHandleUAV());
	commandList->SetComputeRootConstantBufferView(4, config.camera->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(5, config.scene->GetTopLevelAS().GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(6, reservoir_->GetGPUVirtualAddress());

	commandList->SetComputeRootConstantBufferView(7, config.scene->directionalLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(8, config.scene->directionalLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(9, config.scene->directionalLightParams_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(10, config.scene->directionalLightShadowParams_->GetGPUVirtualAddress());

	commandList->SetComputeRootConstantBufferView(11, config.scene->pointLightCount_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(12, config.scene->pointLightTransforms_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(13, config.scene->pointLightParams_->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(14, config.scene->pointLightShadowParams_->GetGPUVirtualAddress());

	config.scene->GetStateObjectContext().DispatchRays(context->GetDxCommand(), textures_->GetSize());

	textures_->EndRaytracingPass(context);
	context->TransitionAllocator();
}
