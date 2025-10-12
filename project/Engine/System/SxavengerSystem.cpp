#include "SxavengerSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	//* system orign
	static std::unique_ptr<WinApp>                sWinApp                = nullptr; //!< win app system
	static std::unique_ptr<DirectXCommon>         sDirectXCommon         = nullptr; //!< DirectX12 system
	static std::unique_ptr<DirectXQueueContext>   sDirectQueueContext    = nullptr; //!< direct queue context
	static std::unique_ptr<Performance>           sPerformance           = nullptr; //!< performance system
	static std::unique_ptr<AsyncThreadCollection> sAsyncThreadCollection = nullptr; //!< async thread system

	//* system user
	static std::unique_ptr<WindowCollection> sWindowCollection  = nullptr; //!< window collection
	static std::unique_ptr<Input>            sInput             = nullptr; //!< input system
	static std::unique_ptr<ImGuiController>  sImGuiController   = nullptr; //!< ui system
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerSystem class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerSystem::Init() {

	Logger::EngineLog("Engine Version: " + kSxavengerEngineVersion);
	Logger::EngineLog(std::format("Build Profile: {}", _PROFILE));
	Logger::EngineThreadLog("this thread is main thread.");

	sWinApp = std::make_unique<WinApp>();
	sWinApp->Init();

	sDirectXCommon = std::make_unique<DirectXCommon>();
	sDirectXCommon->Init();

	sDirectQueueContext = std::make_unique<DirectXQueueContext>();
	sDirectQueueContext->Init(2, DirectXQueueContext::RenderQueue::Direct); //!< allocator count

	sPerformance = std::make_unique<Performance>();

	sAsyncThreadCollection = std::make_unique<AsyncThreadCollection>();
	sAsyncThreadCollection->Init();

	sWindowCollection = std::make_unique<WindowCollection>();
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
	sDirectQueueContext->TransitionAllocator();
}

void SxavengerSystem::ExecuteAllAllocator() {
	sDirectQueueContext->ExecuteAllAllocators();
}

DirectXQueueContext* SxavengerSystem::GetDirectQueueContext() {
	return sDirectQueueContext.get();
}

const std::weak_ptr<DirectXWindowContext> SxavengerSystem::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	// windowの生成
	auto window = sWindowCollection->CreateMainWindow(clientSize, name, clearColor);

	// user system の初期化
	sInput->Init(window.lock().get());
	sImGuiController->Init(window.lock().get());

	return window;
}

const std::weak_ptr<DirectXWindowContext> SxavengerSystem::CreateSubWindow(const Vector2ui& clientSize, const LPCWSTR& name, DirectXWindowContext::ProcessCategory category, const Color4f& clearColor) {
	return sWindowCollection->CreateSubWindow(clientSize, name, category, clearColor);
}

bool SxavengerSystem::ProcessMessage() {
	return sWindowCollection->ProcessMessage();
}

void SxavengerSystem::PresentWindows() {
	sWindowCollection->PresentWindows();
}

DirectXWindowContext* SxavengerSystem::GetMainWindow() {
	return sWindowCollection->GetMainWindow();
}

DirectXWindowContext* SxavengerSystem::GetForcusWindow() {
	return sWindowCollection->GetForcusWindow();
}

WindowCollection* SxavengerSystem::GetWindowCollection() {
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

TimePointd<TimeUnit::second> SxavengerSystem::GetDeltaTime() {
	return sPerformance->GetDeltaTime();
}

TimePointf<TimeUnit::second> SxavengerSystem::GetDeltaTimef() {
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

void SxavengerSystem::PushTaskAndWait(AsyncExecution execution, const AsyncTask::Function& function) {
	std::shared_ptr<AsyncTask> task = std::make_shared<AsyncTask>();
	task->SetFunction(function);
	task->SetTag("wait task");

	sAsyncThreadCollection->PushTask(execution, task);

	task->Wait();
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

void SxavengerSystem::RenderImGui(DirectXQueueContext* context) {
	sImGuiController->Render(context);
}

ImGuiController* SxavengerSystem::GetImGuiController() {
	return sImGuiController.get();
}
