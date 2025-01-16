#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Dev/DeveloperGui.h>

#include "Engine/!Render/FRenderCore.h"
#include "Engine/System/Config/SxavengerDirectory.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateSystem(); });
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {
	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"beta system window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	//* render system *//

	scene_ = std::make_unique<FScene>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->CreateTextures(main_->GetSize());
	renderer_->SetScene(scene_.get());

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();
	renderer_->SetCamera(camera_.get());

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	//model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/human/idle.gltf"));
	//model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/primitive/teapot.obj"));
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/chessBoard/chessBoard.gltf"));
	model_->SetRenderWait(false);
	scene_->AddGeometry(model_.get());

	light_ = std::make_unique<APointLightActor>();
	light_->Init();
	scene_->AddLight(light_.get());

	FRenderCore::GetInstance()->Init();

	//* raytracing system *//

	blob1_ = std::make_unique<DxrObject::RaytracingBlob>();
	blob1_->Create("packages/shaders/raytracingDemo/RaygenerationDemo.hlsl");

	raygeneration_ = std::make_unique<DxrObject::ExportGroup>();
	raygeneration_->ExportRaygeneration(L"mainRaygeneration");

	miss_ = std::make_unique<DxrObject::ExportGroup>();
	miss_->ExportMiss(L"mainMiss");

	blob1_->SetExport(raygeneration_.get());
	blob1_->SetExport(miss_.get());

	//* presenter *//

	presenter_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	presenter_->CreateBlob(kPackagesShaderDirectory / "render/presenter/presenter.vs.hlsl", DxObject::GraphicsShaderType::vs);
	presenter_->CreateBlob(kPackagesShaderDirectory / "render/presenter/presenter.ps.hlsl", DxObject::GraphicsShaderType::ps);
	presenter_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	desc.SetRTVFormat(0, DxObject::kScreenFormat);

	presenter_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	vb_ = std::make_unique<DxObject::VertexDimensionBuffer<std::pair<Vector4f, Vector2f>>>();
	vb_->Create(SxavengerSystem::GetDxDevice(), 3);
	vb_->At(0) = { { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } };
	vb_->At(1) = { { 3.0f, 1.0f, 0.0f }, { 2.0f, 0.0f } };
	vb_->At(2) = { { -1.0f, -3.0f, 0.0f }, { 0.0f, 2.0f } };
	
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
	model_->GetTransform().scale  = { 4.0f, 4.0f, 4.0f };
	model_->GetTransform().rotate *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, 0.01f);
	model_->UpdateMatrix();

	static float frame = 0.0f;

	light_->GetTransform().translate.y = 3.0f;
	light_->GetTransform().translate.x = 1.0f;
	light_->UpdateMatrix();
}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	{
		auto command = SxavengerSystem::GetMainThreadContext()->GetDxCommand();

		presenter_->SetPipeline(command, main_->GetSize());

		D3D12_VERTEX_BUFFER_VIEW vbv = vb_->GetVertexBufferView();
		command->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);

		DxObject::BindBufferDesc desc = {};
		desc.SetHandle("gTexture", renderer_->GetDebugTexture());

		presenter_->BindGraphicsBuffer(command, desc);

		command->GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
