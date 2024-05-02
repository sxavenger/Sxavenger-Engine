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

#include <ComPtr.h>

#include <ExecutionSpeed.h>
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
}

////////////////////////////////////////////////////////////////////////////////////////////
// MyEngine class
////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------------------
Camera3D* MyEngine::camera3D_ = nullptr;
Camera2D* MyEngine::camera2D_ = nullptr;

std::unordered_map<Texture*, TextureWriteInfo> MyEngine::writeTextures_;
const TextureWriteInfo* MyEngine::offscreenInfo_;

//-----------------------------------------------------------------------------------------
// method
//-----------------------------------------------------------------------------------------

void MyEngine::Initialize(int32_t kWindowWidth, int32_t kWindowHeight, const char* kWindowTitle) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウの生成
	{
		sWinApp = WinApp::GetInstance();

		std::wstring titleString = ToWstring(kWindowTitle);
		sWinApp->CreateGameWindow(kWindowWidth, kWindowHeight, titleString.c_str());
	}

	// DirectX12 の初期化
	{
		sDirectXCommon = DirectXCommon::GetInstance();
		sDirectXCommon->Init(sWinApp, kWindowWidth, kWindowHeight);
	}

	// ImGuiManager の初期化
	{
		sImGuiManager = ImGuiManager::GetInstaince();
		sImGuiManager->Init(sWinApp, sDirectXCommon);
	}

	// TextureManager の初期化
	{
		sTextureManager = TextureManager::GetInstance();
		sTextureManager->Init(sDirectXCommon);

		// オフスク用のテクスチャの生成 todo: consoleに移動
		sTextureManager->CreateDummyTexture(kWindowWidth, kWindowHeight, "offscreen");
	}
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

	writeTextures_.clear();

	CoUninitialize();
}

void MyEngine::BeginFrame() {
	ExecutionSpeed::Begin();
	sImGuiManager->Begin();
}

void MyEngine::BeginDraw() {
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
	ExecutionSpeed::End();
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

void MyEngine::SetWriteTexture(Texture* writeTexture, const TextureWriteInfo& info) {
	auto it = writeTextures_.find(writeTexture);
	if (it != writeTextures_.end()) { //!< すでにある
		Console::GetInstance()->SetLog(
			std::format("warning: writeTexture already set. Texture*: 0x{:x}", std::uintptr_t(writeTexture)),
			Console::warningColor
		);
		return;
	}

	writeTextures_[writeTexture] = info;
}

void MyEngine::EraseWriteTexture(Texture* writeTexture) {
	auto it = writeTextures_.find(writeTexture);
	if (it == writeTextures_.end()) { //!< mapにない
		Console::GetInstance()->SetLog(
			std::format("warning: couldn't erase it. Texture*: 0x{:x}", std::uintptr_t(writeTexture)),
			Console::warningColor
		);
		return;
	}

	writeTextures_.erase(writeTexture);
}

const std::unordered_map<Texture*, TextureWriteInfo>& MyEngine::GetWriteTextures() {
	return writeTextures_;
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

const D3D12_GPU_DESCRIPTOR_HANDLE& MyEngine::GetTextureHandleGPU(const std::string& textureKey) {
	assert(sTextureManager != nullptr);
	return sTextureManager->GetHandleGPU(textureKey);
}

Texture* MyEngine::GetTexture(const std::string& textureKey) {
	assert(sTextureManager != nullptr);
	return sTextureManager->GetTexture(textureKey);
}
