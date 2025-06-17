#include "SxavengerSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	//* system orign
	static std::unique_ptr<WinApp>                sWinApp                = nullptr; //!< win app system
	static std::unique_ptr<DirectXCommon>         sDirectXCommon         = nullptr; //!< DirectX12 system
	static std::unique_ptr<DirectXThreadContext>  sMainThreadContext     = nullptr; //!< main thread context
	static std::unique_ptr<Performance>           sPerformance           = nullptr; //!< performance system
	static std::unique_ptr<AsyncThreadCollection> sAsyncThreadCollection = nullptr; //!< async thread system

	//* system user
	static std::unique_ptr<GameWindowCollection> sWindowCollection  = nullptr; //!< window collection
	static std::unique_ptr<Input>                sInput             = nullptr; //!< input system
	static std::unique_ptr<ImGuiController>      sImGuiController   = nullptr; //!< ui system
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerSystem class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerSystem::Init() {

	EngineLog("Engine Version: " + kSxavengerEngineVersion);
	EngineThreadLog("this thread is main thread.");

	sWinApp = std::make_unique<WinApp>();
	sWinApp->Init();

	sDirectXCommon = std::make_unique<DirectXCommon>();
	sDirectXCommon->Init();

	sMainThreadContext = std::make_unique<DirectXThreadContext>();
	sMainThreadContext->Init(3); //!< allocator count

	sPerformance = std::make_unique<Performance>();

	sAsyncThreadCollection = std::make_unique<AsyncThreadCollection>();
	sAsyncThreadCollection->Init();

	sWindowCollection = std::make_unique<GameWindowCollection>();
	sInput            = std::make_unique<Input>();
	sImGuiController  = std::make_unique<ImGuiController>();
}

void SxavengerSystem::Term() {
}

_DXOBJECT Descriptor SxavengerSystem::GetDescriptor(_DXOBJECT DescriptorType type) {
	return sDirectXCommon->GetDesriptorHeaps()->GetDescriptor(type);
}

_DXOBJECT Device* SxavengerSystem::GetDxDevice() {
	return sDirectXCommon->GetDevice();
}

_DXOBJECT DescriptorHeaps* SxavengerSystem::GetDxDescriptorHeaps() {
	return sDirectXCommon->GetDesriptorHeaps();
}

void SxavengerSystem::TransitionAllocator() {
	sMainThreadContext->TransitionAllocator();
}

void SxavengerSystem::ExecuteAllAllocator() {
	sMainThreadContext->ExecuteAllAllocators();
}

DirectXThreadContext* SxavengerSystem::GetMainThreadContext() {
	return sMainThreadContext.get();
}

const std::weak_ptr<GameWindow> SxavengerSystem::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	// windowの生成
	auto window = sWindowCollection->CreateMainWindow(clientSize, name, clearColor);

	// user system の初期化
	sInput->Init(window.lock().get());
	sImGuiController->Init(window.lock().get());

	return window;
}

const std::weak_ptr<GameWindow> SxavengerSystem::TryCreateSubWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {
	return sWindowCollection->TryCreateSubWindow(clientSize, name, clearColor);
}

bool SxavengerSystem::ProcessMessage() {
	return sWindowCollection->ProcessMessage();
}

void SxavengerSystem::PresentAllWindow() {
	sWindowCollection->PresentAllWindow();
}

const std::weak_ptr<GameWindow> SxavengerSystem::GetMainWindow() {
	return sWindowCollection->GetMainWindow();
}

const GameWindow* SxavengerSystem::GetForcusWindow() {
	return sWindowCollection->GetForcusWindow();
}

GameWindowCollection* SxavengerSystem::GetGameWindowCollection() {
	return sWindowCollection.get();
}

bool SxavengerSystem::IsPressKey(KeyId id) {
	return sInput->IsPressKey(id);
}

bool SxavengerSystem::IsTriggerKey(KeyId id) {
	return  sInput->IsTriggerKey(id);
}

bool SxavengerSystem::IsReleaseKey(KeyId id) {
	return sInput->IsReleaseKey(id);
}

Input* SxavengerSystem::GetInput() {
	return sInput.get();
}

void SxavengerSystem::BeginPerformace() {
	sPerformance->Begin();
}

void SxavengerSystem::EndPerformace() {
	sPerformance->End();
}

TimePointf<TimeUnit::second> SxavengerSystem::GetDeltaTime() {
	return sPerformance->GetDeltaTime();
}

void SxavengerSystem::RecordLap(const std::string& name) {
	sPerformance->RecordLap(name);
}

Performance* SxavengerSystem::GetPerformance() {
	return sPerformance.get();
}

void SxavengerSystem::PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task) {
	sAsyncThreadCollection->PushTask(execution, task);
}

void SxavengerSystem::ShutdownAsyncThread() {
	sAsyncThreadCollection->Term();
}

void SxavengerSystem::BeginImGuiFrame() {
	sImGuiController->BeginFrame();
}

void SxavengerSystem::EndImGuiFrame() {
	sImGuiController->EndFrame();
}

void SxavengerSystem::RenderImGui(DirectXThreadContext* context) {
	sImGuiController->Render(context);
}

ImGuiController* SxavengerSystem::GetImGuiController() {
	return sImGuiController.get();
}
