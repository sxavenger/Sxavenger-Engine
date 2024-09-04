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
}

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger class methods
////////////////////////////////////////////////////////////////////////////////////////////

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
		sTextureManager->CreateRenderTexture("offscreen", { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) });
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
	Performance::EndFrame();
}

int SxavengerEngine::ProcessMessage() {
	return sWinApp->ProcessMessage() ? 1 : 0;
}

void SxavengerEngine::BeginScreenDraw() {
	sDirectXRCommon->BeginScreenDraw();
}

void SxavengerEngine::BeginOffscreen(RenderTexture* renderTexture) {
	sDirectXRCommon->BeginOffscreen(renderTexture);
}

void SxavengerEngine::EndOffscreen(RenderTexture* renderTexture) {
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

RenderTexture* SxavengerEngine::CreateRenderTexture(const std::string& key, const Vector2ui& textureSize, const Color4f& clearColor) {
	return sTextureManager->CreateRenderTexture(key, textureSize, clearColor);
}

Texture* SxavengerEngine::LoadTexture(const std::string& filePath) {
	return sTextureManager->LoadTexture(filePath);
}

void SxavengerEngine::ReleaseTexture(const std::string& key) {
	sTextureManager->ReleaseTexture(key);
}

BaseTexture* SxavengerEngine::GetBaseTexture(const std::string& textureKey) {
	return sTextureManager->GetBaseTexture(textureKey);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SxavengerEngine::GetTextureHandleGPU(const std::string& textureKey) {
	return sTextureManager->GetGPUHandleSRV(textureKey);
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

std::unique_ptr<Audio> SxavengerEngine::GetAudio(const std::string& filename, bool isLoop) {
	return sAudioManager->GetAudio(filename, isLoop);
}

void SxavengerEngine::PlayAudioOneShot(const std::string& filename, float volume) {
	sAudioManager->PlayOneShot(filename, volume);
}

AudioManager* SxavengerEngine::GetAudioManager() {
	return sAudioManager;
}
