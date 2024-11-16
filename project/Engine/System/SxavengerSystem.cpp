#include "SxavengerSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	std::unique_ptr<WinApp>               sWinApp            = nullptr; //!< win app system
	std::unique_ptr<DirectXCommon>        sDirectXCommon     = nullptr; //!< DirectX12 system
	std::unique_ptr<DirectXThreadContext> sMainThreadContext = nullptr; //!< main thread context
	std::unique_ptr<GameWindowCollection> sWindowCollection  = nullptr; //!< window collection
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
	sMainThreadContext->Init(2);

	sWindowCollection = std::make_unique<GameWindowCollection>();
}

void SxavengerSystemEngine::Term() {
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

ID3D12GraphicsCommandList6* SxavengerSystemEngine::GetCommandList() {
	return sMainThreadContext->GetCommandList();
}

DirectXThreadContext* SxavengerSystemEngine::GetMainThreadContext() {
	return sMainThreadContext.get();
}

GameWindow* SxavengerSystemEngine::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {
	return sWindowCollection->CreateMainWindow(clientSize, name, clearColor);
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
