#include "Sxavenger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <memory>

//* system
#include "Logger.h"

#include "Performance.h"
#include "Console.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	std::unique_ptr<WinApp>         sWinApp         = nullptr; //!< WindowsApp system
	std::unique_ptr<DirectXCommon>  sDirectXCommon  = nullptr; //!< DirectX12 system
	std::unique_ptr<ImGuiManager>   sImGuiManager   = nullptr; //!< ImGui system
	std::unique_ptr<TextureManager> sTextureManager = nullptr; //!< Texture system
	std::unique_ptr<Input>          sInput          = nullptr; //!< Input system
	std::unique_ptr<AudioManager>   sAudioManager   = nullptr; //!< Audio system
}

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerEngine::Init(const Vector2ui& windowSize, const char* windowTitle) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウの生成
	{
		sWinApp = std::make_unique<WinApp>();

		std::wstring title = ToWstring(windowTitle);
		sWinApp->CreateGameWindow(windowSize, title.c_str());
	}

	// DirectX12 の初期化
	{
		sDirectXCommon = std::make_unique<DirectXCommon>();
		sDirectXCommon->Init(sWinApp.get());
		//sDirectXCommon->InitRayTracing(kWindowWidth, kWindowHeight);
	}

	// ImGuiManager の初期化
	{
		sImGuiManager = std::make_unique<ImGuiManager>();
		sImGuiManager->Init(sWinApp.get(), sDirectXCommon.get());
	}

	// TextureManager の初期化
	{
		sTextureManager = std::make_unique<TextureManager>();
		sTextureManager->Init(sDirectXCommon.get());

		// オフスク用のテクスチャの生成 todo: consoleに移動
		sTextureManager->CreateRenderTexture("main", windowSize);
	}

	// Inputの初期化
	{
		sInput = std::make_unique<Input>();
		sInput->Init(sWinApp->GetHinst(), sWinApp->GetHwnd());
	}

	// Audioの初期化
	{
		sAudioManager = std::make_unique<AudioManager>();
		sAudioManager->Init();
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
	sAudioManager.reset();

	sTextureManager->Term();
	sTextureManager.reset();

	sImGuiManager->Term();
	sImGuiManager.reset();

	// sDirectXCommon->TermRayTracing();
	sDirectXCommon->Term();
	sDirectXCommon.reset();

	sWinApp->Term();
	sWinApp.reset();

	CoUninitialize();
}

void SxavengerEngine::BeginFrame() {
	Performance::BeginFrame();
	sInput->Update();
	sImGuiManager->Begin();
}

void SxavengerEngine::EndFrame() {
	sDirectXCommon->EndFrame();
	Performance::EndFrame();
}

int SxavengerEngine::ProcessMessage() {
	return sWinApp->ProcessMessage() ? 1 : 0;
}

void SxavengerEngine::BeginScreenDraw() {
	sDirectXCommon->BeginScreenDraw();
}

void SxavengerEngine::BeginOffscreen(RenderTexture* renderTexture) {
	sDirectXCommon->BeginOffscreen(renderTexture);
}

void SxavengerEngine::EndOffscreen(RenderTexture* renderTexture) {
	sDirectXCommon->EndOffscreen(renderTexture);
}

void SxavengerEngine::TranstionAllocator() {
	sDirectXCommon->TranstionAllocator();
}

void SxavengerEngine::ResetBackAllocator() {
	sDirectXCommon->ResetBackAllocator();
}

_DXOBJECT Descriptor SxavengerEngine::GetDescriptor(_DXOBJECT DescriptorType type) {
	return sDirectXCommon->GetDescriptorsObj()->GetDescriptor(type);
}

void SxavengerEngine::DeleteDescriptor(_DXOBJECT Descriptor& descriptor) {
	sDirectXCommon->GetDescriptorsObj()->DeleteDescriptor(descriptor);
}

ID3D12Device8* SxavengerEngine::GetDevice() {
	return sDirectXCommon->GetDeviceObj()->GetDevice();
}

ID3D12GraphicsCommandList6* SxavengerEngine::GetCommandList() {
	return sDirectXCommon->GetCommandList();
}

_DXOBJECT Devices* SxavengerEngine::GetDevicesObj() {
	return sDirectXCommon->GetDeviceObj();
}

DirectXCommon* SxavengerEngine::GetDxCommon() {
	return sDirectXCommon.get();
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
	return sTextureManager.get();
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
	return sInput.get();
}

std::unique_ptr<Audio> SxavengerEngine::GetAudio(const std::string& filename, bool isLoop) {
	return sAudioManager->GetAudio(filename, isLoop);
}

void SxavengerEngine::PlayAudioOneShot(const std::string& filename, float volume) {
	sAudioManager->PlayOneShot(filename, volume);
}

AudioManager* SxavengerEngine::GetAudioManager() {
	return sAudioManager.get();
}

void SxavengerEngine::EndImGuiAndDrawCall() {
	sImGuiManager->EndDrawCall();
}
