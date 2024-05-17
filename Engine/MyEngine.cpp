#include "MyEngine.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

// origin
#include <WinApp.h>
#include <DirectXCommon.h>
#include <ImGuiManager.h>
#include <TextureManager.h>
#include <Input.h>

#include <ComPtr.h>

#include <Performance.h>
#include <Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	//-----------------------------------------------------------------------------------------
	// 汎用機能
	//-----------------------------------------------------------------------------------------
	WinApp* sWinApp = nullptr;                 //!< WindowApp system
	DirectXCommon* sDirectXCommon = nullptr;   //!< DirectX12 system
	ImGuiManager* sImGuiManager = nullptr;     //!< ImGui system
	TextureManager* sTextureManager = nullptr; //!< TextureManager system
	Input* sInput = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// MyEngine class
////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------------------
Camera3D* MyEngine::camera3D_ = nullptr;
Camera2D* MyEngine::camera2D_ = nullptr;

//-----------------------------------------------------------------------------------------
// method
//-----------------------------------------------------------------------------------------

void MyEngine::Initialize(int32_t kWindowWidth, int32_t kWindowHeight, const char* kWindowTitle) {
	ExternalLogger::Open();
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウの生成
	{
		sWinApp = WinApp::GetInstance();

		std::wstring titleString = ToWstring(kWindowTitle);

		std::string title = kWindowTitle;
		ExternalLogger::Write("[windowName]: " + title);

		sWinApp->CreateGameWindow(kWindowWidth, kWindowHeight, titleString.c_str());
		ExternalLogger::Write("Complete Initialize: sWinApp");
	}

	// DirectX12 の初期化
	{
		sDirectXCommon = DirectXCommon::GetInstance();
		sDirectXCommon->Init(sWinApp, kWindowWidth, kWindowHeight);
		ExternalLogger::Write("Complete Initialize: sDirectXCommon");

	}

	
	// ImGuiManager の初期化
	{
		sImGuiManager = ImGuiManager::GetInstaince();
		sImGuiManager->Init(sWinApp, sDirectXCommon);
		ExternalLogger::Write("Complete Initialize: sImGuiManager");
	}
	

	// TextureManager の初期化
	{
		sTextureManager = TextureManager::GetInstance();
		sTextureManager->Init(sDirectXCommon);
		ExternalLogger::Write("Complete Initialize: sTextureManager");

		// オフスク用のテクスチャの生成 todo: consoleに移動
		sTextureManager->CreateDummyTexture(kWindowWidth, kWindowHeight, "offscreen");
	}

	// Inputの初期化
	{
		sInput = Input::GetInstance();
		sInput->Init(sWinApp->GetHinst(), sWinApp->GetHwnd());
		ExternalLogger::Write("Complete Initialize: sInput");
	}

	ExternalLogger::Close();
}

void MyEngine::Finalize() {
	sTextureManager->Term();
	sTextureManager = nullptr;

	sImGuiManager->Term();
	sImGuiManager = nullptr;

	sDirectXCommon->Term();
	sDirectXCommon = nullptr;

	sWinApp->Term();
	sWinApp = nullptr;

	CoUninitialize();
}

void MyEngine::BeginFrame() {
	Performance::BeginFrame();
	sInput->Update();
	sImGuiManager->Begin();
}

void MyEngine::BeginScreenDraw() {
	sDirectXCommon->BeginFrame();
}

void MyEngine::BeginOffScreen(Texture* offscreenDummyTexture) {
	sDirectXCommon->BeginOffscreen(offscreenDummyTexture);
}

void MyEngine::EndOffScreen() {
	sDirectXCommon->EndOffscreen();
}

void MyEngine::EndFrame() {
	sImGuiManager->End();
	sDirectXCommon->EndFrame();
	Performance::EndFrame();
}

void MyEngine::BeginDraw() {
	sDirectXCommon->SentTexture();
	sTextureManager->EnableTexture();
}

int MyEngine::ProcessMessage() {
	return sWinApp->ProcessMessage() ? 1 : 0;
}

void MyEngine::SetPipelineType(PipelineType pipelineType) {
	sDirectXCommon->SetPipelineType(pipelineType);
}

void MyEngine::SetBlendMode(BlendMode mode) {
	sDirectXCommon->SetBlendMode(mode);
}

void MyEngine::SetPipelineState() {
	sDirectXCommon->SetPipelineState();
}

DxObject::Descriptor MyEngine::GetCurrentDescripor(DxObject::DescriptorType type) {
	assert(sDirectXCommon != nullptr);
	return sDirectXCommon->GetDescriptorsObj()->GetCurrentDescriptor(type);
}

void MyEngine::EraseDescriptor(DxObject::Descriptor& descriptor) {
	assert(sDirectXCommon != nullptr);
	sDirectXCommon->GetDescriptorsObj()->Erase(descriptor);
}

ID3D12GraphicsCommandList6* MyEngine::GetCommandList() {
	assert(sDirectXCommon != nullptr);
	return sDirectXCommon->GetCommandList();
}

DxObject::Devices* MyEngine::GetDevicesObj() {
	assert(sDirectXCommon != nullptr);
	return sDirectXCommon->GetDeviceObj();
}

DirectXCommon* MyEngine::GetDxCommon() {
	assert(sDirectXCommon != nullptr);
	return sDirectXCommon;
}

TextureManager* MyEngine::GetTextureManager() {
	assert(sTextureManager != nullptr);
	return sTextureManager;
}

Texture* MyEngine::CreateDummyTexture(int32_t width, int32_t height, const std::string& key) {
	sTextureManager->CreateDummyTexture(width, height, key);
	return sTextureManager->GetTexture(key);
}

void MyEngine::LoadTexture(const std::string& filePath) {
	assert(sTextureManager != nullptr);
	sTextureManager->LoadTexture(filePath);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& MyEngine::GetTextureHandleGPU(const std::string& textureKey) {
	assert(sTextureManager != nullptr);
	return sTextureManager->GetHandleGPU(textureKey);
}

Texture* MyEngine::GetTexture(const std::string& textureKey) {
	assert(sTextureManager != nullptr);
	return sTextureManager->GetTexture(textureKey);
}

bool MyEngine::IsPressKey(uint8_t dik) {
	assert(sInput != nullptr);
	return sInput->IsPressKey(dik);
}

bool MyEngine::IsTriggerKey(uint8_t dik) {
	assert(sInput != nullptr);
	return sInput->IsTriggerKey(dik);
}

bool MyEngine::IsReleaseKey(uint8_t dik) {
	assert(sInput != nullptr);
	return sInput->IsReleaseKey(dik);
}
