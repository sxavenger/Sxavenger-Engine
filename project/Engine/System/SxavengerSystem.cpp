#include "SxavengerSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	//* system orign
	static std::unique_ptr<WinApp>                sWinApp            = nullptr; //!< win app system
	static std::unique_ptr<DirectXCommon>         sDirectXCommon     = nullptr; //!< DirectX12 system
	static std::unique_ptr<DirectXThreadContext>  sMainThreadContext = nullptr; //!< main thread context

	//* system user
	static std::unique_ptr<GameWindowCollection> sWindowCollection  = nullptr; //!< window collection
	static std::unique_ptr<Input>                sInput             = nullptr; //!< input system
	static std::unique_ptr<ImGuiController>      sImGuiController   = nullptr; //!< ui system
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerSystemEngine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerSystemEngine::Init() {

	sWinApp = std::make_unique<WinApp>();
	sWinApp->Init();

	sDirectXCommon = std::make_unique<DirectXCommon>();
	sDirectXCommon->Init();

	sMainThreadContext = std::make_unique<DirectXThreadContext>();
	sMainThreadContext->Init(3); //!< allocator count

	sWindowCollection = std::make_unique<GameWindowCollection>();
	sInput            = std::make_unique<Input>();
	sImGuiController  = std::make_unique<ImGuiController>();
}

void SxavengerSystemEngine::Term() {
}

_DXOBJECT Descriptor SxavengerSystemEngine::GetDescriptor(_DXOBJECT DescriptorType type) {
	return sDirectXCommon->GetDesriptorHeaps()->GetDescriptor(type);
}

_DXOBJECT Device* SxavengerSystemEngine::GetDxDevice() {
	return sDirectXCommon->GetDevice();
}

_DXOBJECT DescriptorHeaps* SxavengerSystemEngine::GetDxDescriptorHeaps() {
	return sDirectXCommon->GetDesriptorHeaps();
}

void SxavengerSystemEngine::TransitionAllocator() {
	sMainThreadContext->TransitionAllocator();
}

void SxavengerSystemEngine::ExecuteAllAllocator() {
	sMainThreadContext->ExecuteAllAllocators();
}

//ID3D12GraphicsCommandList6* SxavengerSystemEngine::GetCommandList() {
//	return sMainThreadContext->GetCommandList();
//}

DirectXThreadContext* SxavengerSystemEngine::GetMainThreadContext() {
	return sMainThreadContext.get();
}

const std::weak_ptr<GameWindow> SxavengerSystemEngine::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	// windowの生成
	auto window = sWindowCollection->CreateMainWindow(clientSize, name, clearColor);

	// user system の初期化
	sInput->Init(window.lock().get());
	sImGuiController->Init(window.lock().get());

	return window;
}

const std::weak_ptr<GameWindow> SxavengerSystemEngine::TryCreateSubWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {
	return sWindowCollection->TryCreateSubWindow(clientSize, name, clearColor);
}

bool SxavengerSystemEngine::ProcessMessage() {
	return sWindowCollection->ProcessMessage();
}

void SxavengerSystemEngine::PresentAllWindow() {
	sWindowCollection->PresentAllWindow();
}

const std::weak_ptr<GameWindow> SxavengerSystemEngine::GetMainWindow() {
	return sWindowCollection->GetMainWindow();
}

const GameWindow* SxavengerSystemEngine::GetForcusWindow() {
	return sWindowCollection->GetForcusWindow();
}

GameWindowCollection* SxavengerSystemEngine::GetGameWindowCollection() {
	return sWindowCollection.get();
}

bool SxavengerSystemEngine::IsPressKey(KeyId id) {
	return sInput->IsPressKey(id);
}

bool SxavengerSystemEngine::IsTriggerKey(KeyId id) {
	return  sInput->IsTriggerKey(id);
}

bool SxavengerSystemEngine::IsReleaseKey(KeyId id) {
	return sInput->IsReleaseKey(id);
}

Input* SxavengerSystemEngine::GetInput() {
	return sInput.get();
}

void SxavengerSystemEngine::BeginImGuiFrame() {
	sImGuiController->BeginFrame();
}

void SxavengerSystemEngine::EndImGuiFrame() {
	sImGuiController->EndFrame();
}

void SxavengerSystemEngine::RenderImGui(DirectXThreadContext* context) {
	sImGuiController->Render(context);
}

ImGuiController* SxavengerSystemEngine::GetImGuiController() {
	return sImGuiController.get();
}
