#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/Console/Console.h>

//* lib
#include <Lib/Environment.h>

#include "Lib/Adapter/Json/JsonAdapter.h"


////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//-----------------------------------------------------------------------------------------
	// 初期化処理
	//-----------------------------------------------------------------------------------------
	sConsole->Init();
	Init();
	SxavengerSystem::ExecuteAllAllocator();

	//-----------------------------------------------------------------------------------------
	// ゲームループ
	//-----------------------------------------------------------------------------------------
	while (SxavengerSystem::ProcessMessage()) {

		Performance::BeginFrame();
		SxavengerSystem::BeginImGuiFrame();

		//-----------------------------------------------------------------------------------------
		// 更新処理
		//-----------------------------------------------------------------------------------------

		SxavengerSystem::GetInput()->Update();
		sConsole->UpdateConsole();

		if (sConsole->IsUpdateRequired()) {
			Update();
		}

		SxavengerSystem::EndImGuiFrame();
		SxavengerSystem::TransitionAllocator();

		//-----------------------------------------------------------------------------------------
		// 描画処理
		//-----------------------------------------------------------------------------------------

		Draw();

		SxavengerSystem::PresentAllWindow();
		SxavengerSystem::ExecuteAllAllocator();
		//!< sub window delete時に死ぬ
		//!< shader hot reload 時に死ぬ

		Performance::EndFrame();

		if (SxavengerSystem::IsTriggerKey(KeyId::KEY_F4)) {
			break;
		}
	}

	//-----------------------------------------------------------------------------------------
	// 終了処理
	//-----------------------------------------------------------------------------------------

	Term();
	sConsole->Term();

}

void GameScene::Init() {
	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle);
	mainWindow_->SetIcon("resources/icon/SxavengerEngineIcon.ico", { 32, 32 });

	subWindow_ = SxavengerSystem::TryCreateSubWindow({ 400, 400 }, L"sub", ToColor4f(0x3A504BFF));

	input_.Create(3, 3);
	(*input_.GetVertex())[0] = { 0.0f, 0.1f, 0.1f };
	(*input_.GetVertex())[1] = { 0.1f, -0.1f, 0.1f };
	(*input_.GetVertex())[2] = { -0.1f, -0.1f, 0.1f };

	(*input_.GetIndex())[0] = 0;
	(*input_.GetIndex())[1] = 1;
	(*input_.GetIndex())[2] = 2;

	state_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	state_->CreateBlob(L"simple.vs.hlsl", DxObject::GraphicsShaderType::vs);
	state_->CreateBlob(L"simple.ps.hlsl", DxObject::GraphicsShaderType::ps);

	state_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetDepthStencil(true);

	desc.SetBlendMode(0, BlendMode::kBlendModeNormal);

	desc.SetPrimitive(DxObject::PrimitiveType::kTriangle);

	desc.SetRTVFormat(DxObject::kScreenFormat);
	desc.SetDSVFormat(DxObject::kDefaultDepthFormat);

	state_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	transform_.CreateBuffer();
	transform_.GetTransform().translate = { 0.0f, -1.0f, 0.0f };
	transform_.UpdateMatrix();

	compute_ = std::make_unique<DxObject::ReflectionComputePipelineState>();
	compute_->CreateBlob(L"common/white1x1.cs.hlsl");
	compute_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	texture_ = std::make_unique<UnorderedTexture>();
	texture_->Create({ 1, 1 });

	model_ = std::make_unique<Model>();
	model_->AsyncLoad("resources/model/kipfel", "kipfel.fbx", Model::GetDefaultAssimpOption() | aiProcess_Triangulate);
}

void GameScene::Update() {

	transform_.GetTransform().translate.y += 0.01f;
	transform_.UpdateMatrix();

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		state_->ReloadShader();
		compute_->ReloadShader();
	}

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_P)) {
		renderWindowSwitch_ = !renderWindowSwitch_;
	}


}

void GameScene::Draw() {

	texture_->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());

	compute_->ReloadAndSetPipeline(SxavengerSystem::GetMainThreadContext()->GetDxCommand());

	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gOutput", texture_->GetGPUHandleUAV());

	compute_->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), bind);
	compute_->Dispatch(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), 1, 1, 1);

	texture_->TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());

	if (!subWindow_.expired()) {
		auto window = subWindow_.lock();

		window->BeginRendering();
		window->ClearWindow();

		if (!renderWindowSwitch_) {

			state_->ReloadAndSetPipeline(SxavengerSystem::GetCommandList(), window->GetSize());

			DxObject::BindBufferDesc desc = {};
			desc.SetAddress("gTransform", transform_.GetGPUVirtualAddress());
			desc.SetHandle("gTexture", texture_->GetGPUHandleSRV());

			state_->BindGraphicsBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);

			input_.BindIABuffer();
			input_.DrawCall();

			SxavengerSystem::RenderImGui();
		}

		window->EndRendering();
	}

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();

	if (renderWindowSwitch_) {

		state_->ReloadAndSetPipeline(SxavengerSystem::GetCommandList(), mainWindow_->GetSize());

		DxObject::BindBufferDesc desc = {};
		desc.SetAddress("gTransform", transform_.GetGPUVirtualAddress());
		desc.SetHandle("gTexture", texture_->GetGPUHandleSRV());

		state_->BindGraphicsBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), desc);

		input_.BindIABuffer();
		input_.DrawCall();

		SxavengerSystem::RenderImGui();
	}

	mainWindow_->EndRendering();
}

void GameScene::Term() {
	SxavengerSystem::ExecuteAllAllocator();
}


