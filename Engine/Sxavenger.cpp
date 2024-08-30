#include "Sxavenger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>
#include <Performance.h>
#include <Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	WinApp*         sWinApp         = nullptr; //!< WindowsApp system
	DirectXCommon*  sDirectXRCommon = nullptr; //!< DirectX12 system
	ImGuiManager*   sImGuiManager   = nullptr; //!< ImGui system
	TextureManager* sTextureManager = nullptr; //!< Texture system
	Input*          sInput          = nullptr; //!< Input system
	AudioManager*   sAudioManager   = nullptr; //!< Audio system

	PrimitiveDrawer* sPrimitive = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger class
////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------------------
Camera3D* Sxavenger::camera3D = nullptr;
Camera2D* Sxavenger::camera2D = nullptr;

//-----------------------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------------------

void SxavengerEngine::Init(int32_t windowWidth, int32_t windowHeight, const char* windowTitle) {
	ExternalLogger::Open();
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウの生成
	{
		sWinApp = WinApp::GetInstance();

		std::wstring titleString = ToWstring(windowTitle);

		std::string title = windowTitle;
		ExternalLogger::Write("[windowName]: " + title);

		sWinApp->CreateGameWindow(windowWidth, windowHeight, titleString.c_str());
		ExternalLogger::Write("Complete Initialize: sWinApp");
	}

	// DirectX12 の初期化
	{
		sDirectXRCommon = DirectXCommon::GetInstance();
		sDirectXRCommon->Init(sWinApp, windowWidth, windowHeight);
		//sDirectXRCommon->InitRayTracing(kWindowWidth, kWindowHeight);
		ExternalLogger::Write("Complete Initialize: sDirectXRCommon");

	}


	// ImGuiManager の初期化
	{
		sImGuiManager = ImGuiManager::GetInstaince();
		sImGuiManager->Init(sWinApp, sDirectXRCommon);
		ExternalLogger::Write("Complete Initialize: sImGuiManager");
	}


	// TextureManager の初期化
	{
		sTextureManager = TextureManager::GetInstance();
		sTextureManager->Init(sDirectXRCommon);
		ExternalLogger::Write("Complete Initialize: sTextureManager");

		// オフスク用のテクスチャの生成 todo: consoleに移動
		sTextureManager->CreateRenderTexture("offscreen", windowWidth, windowHeight);
	}

	// Inputの初期化
	{
		sInput = Input::GetInstance();
		sInput->Init(sWinApp->GetHinst(), sWinApp->GetHwnd());
		ExternalLogger::Write("Complete Initialize: sInput");
	}

	// Audioの初期化
	{
		sAudioManager = AudioManager::GetInstance();
		sAudioManager->Init();
		ExternalLogger::Write("Complete Initialize: sAudioManager");
	}

	// PrimitiveDrawerの初期化
	{
		sPrimitive = PrimitiveDrawer::GetInstance();
		sPrimitive->Init();
		ExternalLogger::Write("Complete Initialize: sPrimitive");
	}

	// PerformanceのBufferの生成
	{
		Performance::CreateBuffer();
	}

	ExternalLogger::Close();
}

void SxavengerEngine::Term() {
	Performance::TermBuffer();

	sAudioManager->Term();
	sAudioManager = nullptr;

	sTextureManager->Term();
	sTextureManager = nullptr;

	sImGuiManager->Term();
	sImGuiManager = nullptr;

	// sDirectXRCommon->TermRayTracing();
	sDirectXRCommon->Term();
	sDirectXRCommon = nullptr;

	sWinApp->Term();
	sWinApp = nullptr;

	CoUninitialize();
}

void SxavengerEngine::BeginFrame() {
	Performance::BeginFrame();
	sInput->Update();
	sImGuiManager->Begin();
}

void SxavengerEngine::EndFrame() {
	sImGuiManager->End();
	sDirectXRCommon->EndFrame();
	sPrimitive->ResetObjectCount();
	Performance::EndFrame();
}

int SxavengerEngine::ProcessMessage() {
	return sWinApp->ProcessMessage() ? 1 : 0;
}

void SxavengerEngine::BeginScreenDraw() {
	sDirectXRCommon->BeginScreenDraw();
}

void SxavengerEngine::BeginOffscreen(Texture* renderTexture) {
	sDirectXRCommon->BeginOffscreen(renderTexture);
}

void SxavengerEngine::EndOffscreen(Texture* renderTexture) {
	sDirectXRCommon->EndOffscreen(renderTexture);
}

void SxavengerEngine::BeginOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {
	sDirectXRCommon->BeginOffscreens(textureNum, renderTextures);
}

void SxavengerEngine::EndOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {
	sDirectXRCommon->EndOffscreens(textureNum, renderTextures);
}

void SxavengerEngine::TransitionProcess() {
	sDirectXRCommon->TransitionProcess();
}

void SxavengerEngine::TransitionProcessSingle() {
	sDirectXRCommon->TransitionSingleAllocator();
}

_DXOBJECT Descriptor SxavengerEngine::GetCurrentDescriptor(_DXOBJECT DescriptorType type) {
	return sDirectXRCommon->GetDescriptorsObj()->GetDescriptor(type);
}

void SxavengerEngine::DeleteDescriptor(_DXOBJECT Descriptor& descriptor) {
	sDirectXRCommon->GetDescriptorsObj()->DeleteDescriptor(descriptor);
}

ID3D12Device8* SxavengerEngine::GetDevice() {
	return sDirectXRCommon->GetDeviceObj()->GetDevice();
}

ID3D12GraphicsCommandList6* SxavengerEngine::GetCommandList() {
	return sDirectXRCommon->GetCommandList();
}

_DXOBJECT Devices* SxavengerEngine::GetDevicesObj() {
	return sDirectXRCommon->GetDeviceObj();
}

DirectXCommon* SxavengerEngine::GetDxCommon() {
	return sDirectXRCommon;
}

Texture* SxavengerEngine::CreateRenderTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight, const Color4f& clearColor) {
	return sTextureManager->CreateRenderTexture(key, textureWidth, textureHeight, clearColor);
}

Texture* SxavengerEngine::LoadTexture(const std::string& filePath) {
	return sTextureManager->LoadTexture(filePath);
}

void SxavengerEngine::ReleaseTexture(const std::string& key) {
	sTextureManager->ReleaseTexture(key);
}

Texture* SxavengerEngine::GetTexture(const std::string& textureKey) {
	return sTextureManager->GetTexture(textureKey);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SxavengerEngine::GetTextureHandleGPU(const std::string& textureKey) {
	return sTextureManager->GetGPUHandle(textureKey);
}

TextureManager* SxavengerEngine::GetTextureManager() {
	return sTextureManager;
}

bool SxavengerEngine::IsPressKey(uint8_t dik) {
	return sInput->IsPressKey(dik);
}

bool SxavengerEngine::IsTriggerKey(uint8_t dik) {
	return sInput->IsTriggerKey(dik);
}

bool SxavengerEngine::IsReleaseKey(uint8_t dik) {
	return sInput->IsReleaseKey(dik);
}

Input* SxavengerEngine::GetInput() {
	return sInput;
}

AudioManager* SxavengerEngine::GetAudioManager() {
	return sAudioManager;
}
