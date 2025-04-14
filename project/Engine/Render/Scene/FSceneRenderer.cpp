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

	RenderGeometryPass(context, conf);

	LightingPass(context, conf);

	RenderTransparentBasePass(context, conf);

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
		WarningRuntime("warning | [FSceneRenderer]::Status", "textures is nullptr.");
		status |= Status::Error_Textures;
	}

	if (config.camera == nullptr) {
		WarningRuntime("warning | [FSceneRenderer]::Status", "camera is nullptr.");
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

		if (!renderer->IsView()) {
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

void FSceneRenderer::RenderGeometryStaticMesh(const DirectXThreadContext* context, const Config& config) {
	// mesh renderer container(BaseComponent)の取得
	auto& container = sComponentStorage->GetComponentContainer<MeshRendererComponent>();

	// InputMeshごとに分ける
	// todo: mesh renderer componentの登録をcomponentが作成された際に行うように変更
	static std::unordered_map<const InputMesh*, std::vector<MeshRendererComponent*>> map;
	map.clear();

	// todo: 実行時間の早い方に変更
	std::for_each(std::execution::seq, container.begin(), container.end(), [&](auto& component) {
		// renderer componentの取得
		MeshRendererComponent* renderer = static_cast<MeshRendererComponent*>(component.get());

		if (!renderer->IsView()) {
			return;
		}

		if (renderer->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			// 透明なジオメトリは別のパスで描画
			return;
		}

		// componentの登録
		map[renderer->GetMesh()].emplace_back(renderer);
	});

	// instance描画用のバッファを作成
	if (transforms_ == nullptr) {
		transforms_ = std::make_unique<DxObject::VectorDimensionBuffer<TransformationMatrix>>();
	}

	if (materials_ == nullptr) {
		materials_ = std::make_unique<DxObject::VectorDimensionBuffer<Material::MaterialBuffer>>();
	}

	auto core = FRenderCore::GetInstance()->GetGeometry();

	// メッシュごとに描画
	for (const auto& [mesh, components] : map) {
		textures_->SetupGeometryPass(context);

		transforms_->Resize(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(components.size()));
		materials_->Resize(SxavengerSystem::GetDxDevice(), static_cast<uint32_t>(components.size()));

		std::for_each(std::execution::seq, components.begin(), components.end(), [&](auto& component) {
			size_t index = &component - std::to_address(components.begin());
			materials_->At(index) = components[index]->GetMaterial()->GetBuffer();
			transforms_->At(index).Transfer(components[index]->GetTransform()->GetMatrix());
		});

		mesh->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", transforms_->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  materials_->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		mesh->DrawCall(context, static_cast<uint32_t>(components.size()));

		context->ExecuteAllAllocators();
		// todo: allocatorごとに一時bufferを作成
	}
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
	parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());

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

	sComponentStorage->ForEach<DirectionalLightComponent>([&](DirectionalLightComponent* component) {
		if (!component->IsActive()) {
			return;
		}

		DxObject::BindBufferDesc parameter = {};
		// common parameter
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

		// deferred paraemter
		parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
		parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());

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

	sComponentStorage->ForEach<PointLightComponent>([&](PointLightComponent* component) {
		if (!component->IsActive()) {
			return;
		}

		DxObject::BindBufferDesc parameter = {};
		// common parameter
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

		// deferred paraemter
		parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
		parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());

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

void FSceneRenderer::LightingPassSkyLight(const DirectXThreadContext* context, const Config& config) {

	FRenderCore::GetInstance()->GetLight()->SetPipeline(
		FRenderCoreLight::LightType::SkyLight, context, textures_->GetSize()
	);

	FRenderCore::GetInstance()->GetLight()->BindIABuffer(context);

	sComponentStorage->ForEach<SkyLightComponent>([&](SkyLightComponent* component) {
		if (!component->IsActive()) {
			return;
		}

		DxObject::BindBufferDesc parameter = {};
		// common parameter
		parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());
		parameter.SetAddress("gScene",  config.scene->GetTopLevelAS().GetGPUVirtualAddress());

		// deferred paraemter
		parameter.SetHandle("gDepth",    textures_->GetDepth()->GetRasterizerGPUHandleSRV());
		parameter.SetHandle("gAlbedo",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
		parameter.SetHandle("gNormal",   textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Normal)->GetGPUHandleSRV());
		parameter.SetHandle("gPosition", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Position)->GetGPUHandleSRV());
		parameter.SetHandle("gMaterial", textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Material_AO)->GetGPUHandleSRV());

		// sky light parameter
		parameter.SetAddress("gDiffuseParameter",  component->GetDiffuseParameterBufferAddress());
		parameter.SetAddress("gSpecularParameter", component->GetSpecularParameterBufferAddress());
		parameter.SetHandle("gBRDFLut",            FRenderCore::GetInstance()->GetBRDFLut());

		//parameter.SetAddress("gShadow",    component->GetShadowBufferAddress());

		FRenderCore::GetInstance()->GetLight()->BindGraphicsBuffer(
			FRenderCoreLight::LightType::SkyLight, context, parameter
		);

		FRenderCore::GetInstance()->GetLight()->DrawCall(context);
	});
}

void FSceneRenderer::RenderTransparentBasePass(const DirectXThreadContext* context, const Config& config) {

	textures_->BeginTransparentBasePass(context);

	RenderTransparentBaseStaticMesh(context, config);

	textures_->EndTransparentBasePass(context);
}

void FSceneRenderer::RenderTransparentBaseStaticMesh(const DirectXThreadContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();

	// componentを取得
	sComponentStorage->ForEach<MeshRendererComponent>([&](MeshRendererComponent* component) {

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
