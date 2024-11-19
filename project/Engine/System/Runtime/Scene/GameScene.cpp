#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//-----------------------------------------------------------------------------------------
	// 初期化処理
	//-----------------------------------------------------------------------------------------
	Init();

	//-----------------------------------------------------------------------------------------
	// ゲームループ
	//-----------------------------------------------------------------------------------------
	while (SxavengerSystem::ProcessMessage()) {

		//-----------------------------------------------------------------------------------------
		// 更新処理
		//-----------------------------------------------------------------------------------------

		Update();

		//-----------------------------------------------------------------------------------------
		// 描画処理
		//-----------------------------------------------------------------------------------------

		Draw();

		if (SxavengerSystem::IsTriggerKey(KeyId::KEY_F4)) {
			break;
		}
	}

	//-----------------------------------------------------------------------------------------
	// 終了処理
	//-----------------------------------------------------------------------------------------

	Term();

}

void GameScene::Init() {
	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle);
	mainWindow_->SetIcon("resources/icon/SxavengerEngineIcon.ico", { 32, 32 });

	subWindow_ = SxavengerSystem::TryCreateSubWindow({ 400, 400 }, L"sub", ToColor4f(0x3A504BFF));

	SxavengerSystem::GetInput()->Init(mainWindow_);

	input_.Create(3, 3);
	(*input_.GetVertex())[0] = { 0.0f, 0.1f, 0.1f };
	(*input_.GetVertex())[1] = { 0.1f, -0.1f, 0.1f };
	(*input_.GetVertex())[2] = { -0.1f, -0.1f, 0.1f };

	(*input_.GetIndex())[0] = 0;
	(*input_.GetIndex())[1] = 1;
	(*input_.GetIndex())[2] = 2;

	state_ = std::make_unique<DxObject::GraphicsPipelineState>();
	state_->CreateBlob(L"simple.vs.hlsl", DxObject::GraphicsShaderType::vs);
	state_->CreateBlob(L"simple.ps.hlsl", DxObject::GraphicsShaderType::ps);

	state_->CreateRootSignature(SxavengerSystem::GetDxDevice(), {});

	DxObject::GraphicsPipelineDesc desc = {};
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetDepthStencil(true);

	desc.SetBlendMode(0, BlendMode::kBlendModeNormal);

	desc.SetPrimitive(DxObject::PrimitiveType::Triangle);

	desc.SetRTVFormat(DxObject::kScreenFormat);
	desc.SetDSVFormat(DxObject::kDefaultDepthFormat);

	desc.SetViewport(kMainWindowSize);

	state_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void GameScene::Update() {

	SxavengerSystem::GetInput()->Update();

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		state_->HotReloadShader();
	}

	SxavengerSystem::TransitionAllocator();
}

void GameScene::Draw() {

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();
 
	state_->SetPipeline(SxavengerSystem::GetCommandList());

	input_.BindIABuffer();
	input_.DrawCall();

	mainWindow_->EndRendering();

	if (!subWindow_.expired()) {
		auto window = subWindow_.lock();

		window->BeginRendering();
		window->ClearWindow();
		window->EndRendering();
	}

	SxavengerSystem::PresentAllWindow();
	SxavengerSystem::ExecuteAllAllocator();
}

void GameScene::Term() {
	SxavengerSystem::ExecuteAllAllocator();
}


