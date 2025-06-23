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
		textures_->ClearTextures(context);
	}

	if (status.Any(Status::Status_Error)) {
		return;
	}

	switch (config.technique) {
		case GraphicsTechnique::Deferred:
			RenderTechniqueDeferred(context, conf);
			break;

		case GraphicsTechnique::Raytracing:
			RenderTechniqueRaytracing(context, conf);
			break;
	}
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
	// mesh renderer container(BaseComponent)の取得
	auto& container = sComponentStorage->GetComponentContainer<MeshRendererComponent>();

	auto core = FRenderCore::GetInstance()->GetGeometry();

	std::for_each(std::execution::seq, container.begin(), container.end(), [&](auto& component) {
		// renderer componentの取得
		MeshRendererComponent* renderer = static_cast<MeshRendererComponent*>(component.get());

		if (!(renderer->IsActive() && renderer->IsView())) {
			return;
		}

		if (renderer->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			// 透明なジオメトリは別のパスで描画
			return;
		}

		renderer->GetMesh()->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", renderer->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  renderer->GetMaterial()->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		renderer->GetMesh()->DrawCall(context, 1);
	});

	context->TransitionAllocator();
}

void FSceneRenderer::RenderGeometrySkinnedMesh(const DirectXThreadContext* context, const Config& config) {

	// mesh renderer container(BaseComponent)の取得
	auto& container = sComponentStorage->GetComponentContainer<SkinnedMeshRendererComponent>();

	auto core = FRenderCore::GetInstance()->GetGeometry();

	std::for_each(std::execution::seq, container.begin(), container.end(), [&](auto& component) {

		textures_->SetupGeometryPass(context);

		// renderer componentの取得
		SkinnedMeshRendererComponent* renderer = static_cast<SkinnedMeshRendererComponent*>(component.get());

		if (!renderer->IsView()) {
			return;
		}

		if (renderer->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			// 透明なジオメトリは別のパスで描画
			return;
		}

		renderer->BindIABuffer(context);

		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", renderer->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  renderer->GetMaterial()->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		renderer->DrawCall(context, 1); //!< instance数は必ず1

		context->ExecuteAllAllocators();
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

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

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

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Directional, context, textures_->GetSize()
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {

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
		parameter.SetAddress("gTransforms", component->GetTransform()->GetGPUVirtualAddress());
		parameter.SetAddress("gParameter",  component->GetParameterBufferAddress());
		parameter.SetAddress("gShadow",     component->GetShadowBufferAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::Directional, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});
}

void FSceneRenderer::LightingPassPointLight(const DirectXThreadContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Point, context, textures_->GetSize()
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {

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
			FRenderCoreLight::LightType::Point, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});

}

void FSceneRenderer::LightingPassRectLight(const DirectXThreadContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::Rect, context, textures_->GetSize()
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

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

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

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
		parameter.SetHandle("gBRDFLut",            FRenderCore::GetInstance()->GetBRDFLut());
		parameter.SetAddress("gShadow",            component->GetShadowBufferAddress());

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
		component->Process(context, textures_, config.camera);
	});
	//!< HACK: componentのtagなどを作成.

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
	commandList->SetComputeRootDescriptorTable(1, textures_->GetDepth()->GetRaytracingGPUHandleUAV());
	commandList->SetComputeRootConstantBufferView(2, config.camera->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(3, config.scene->GetTopLevelAS().GetGPUVirtualAddress());

	config.scene->GetStateObjectContext().DispatchRays(context->GetDxCommand(), textures_->GetSize());

	textures_->EndRaytracingPass(context);
}
