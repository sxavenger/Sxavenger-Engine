#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Render(const DirectXThreadContext* context) {
	Sxl::Flag<Status> status = CheckStatus();

	// texturesの確認
	if (!status.Test(Status::Error_Textures)) {
		textures_->ClearTextures(context);
	}

	if (status.Any(Status::Status_Error)) {
		return;
	}

	RenderGeometryPass(context);
}

Sxl::Flag<FSceneRenderer::Status> FSceneRenderer::CheckStatus() const {

	Sxl::Flag<Status> status = Status::Success;

	if (textures_ == nullptr) {
		status |= Status::Error_Textures;
	}

	if (camera_ == nullptr) {
		status |= Status::Error_Camera;
	}

	return status;
}

void FSceneRenderer::RenderGeometryPass(const DirectXThreadContext* context) {

	// mesh renderer container(BaseComponent)の取得
	auto& container = sComponentStorage->GetComponentContainer<MeshRendererComponent>();

	// InputMeshごとに分ける
	// todo: mesh renderer componentの登録をcomponentが作成された際に行うように変更
	std::unordered_map<const InputMesh*, std::vector<MeshRendererComponent*>> map;

	// todo: 実行時間の早い方に変更
	std::for_each(std::execution::seq, container.begin(), container.end(), [&](auto& component) {
		// renderer componentの取得
		MeshRendererComponent* renderer = static_cast<MeshRendererComponent*>(component.get());

		if (renderer->GetMaterial()->GetBlendMode() != Material::BlendMode::Opaque) {
			// 透明なジオメトリは別のパスで描画
			return;
		}

		// componentの登録
		map[renderer->GetMesh()].emplace_back(renderer);
	});

	textures_->BeginGeometryPass(context);

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
			materials_->At(index) = components[index]->GetMaterial()->GetMaterial();
			transforms_->At(index).Transfer(components[index]->GetTransform()->GetMatrix());
		});

		mesh->BindIABuffer(context);

		// メッシュの描画
		core->SetPipeline(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, textures_->GetSize()
		);

		DxObject::BindBufferDesc parameter = {};
		parameter.SetAddress("gCamera",     camera_->GetGPUVirtualAddress());
		parameter.SetAddress("gTransforms", transforms_->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  materials_->GetGPUVirtualAddress());

		core->BindGraphicsBuffer(
			FRenderCoreGeometry::Deffered, FRenderCoreGeometry::VertexStage::DefaultVS, FRenderCoreGeometry::PixelStage::Albedo,
			context, parameter
		);

		mesh->DrawCall(context, static_cast<uint32_t>(components.size()));

		context->TransitionAllocator();
	}

	textures_->EndGeometryPass(context);

}
