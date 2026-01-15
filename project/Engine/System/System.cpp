#include "System.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymous-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	//* system origin
	static std::unique_ptr<DirectXCommon>         sDirectXCommon         = nullptr; //!< DirectX12 system
	static std::unique_ptr<DirectXQueueContext>   sDirectQueueContext    = nullptr; //!< direct queue context
	static std::unique_ptr<Performance>           sPerformance           = nullptr; //!< performance system
	static std::unique_ptr<CpuTimestamp>          sCpuTimestamp          = nullptr; //!< cpu timestamp system
	static std::unique_ptr<AsyncThreadCollection> sAsyncThreadCollection = nullptr; //!< async thread system

	//* system user
	static std::unique_ptr<WindowCollection> sWindowCollection  = nullptr; //!< window collection
	static std::unique_ptr<Input>            sInput             = nullptr; //!< input system
	static std::unique_ptr<ImGuiController>  sImGuiController   = nullptr; //!< ui system
}

////////////////////////////////////////////////////////////////////////////////////////////
// System class methods
////////////////////////////////////////////////////////////////////////////////////////////

void System::Init() {

	WinApp::Init();

	sDirectXCommon = std::make_unique<DirectXCommon>();
	sDirectXCommon->Init();

	sDirectQueueContext = std::make_unique<DirectXQueueContext>();
	sDirectQueueContext->Init(2, DirectXQueueContext::RenderQueue::Direct);
	sDirectQueueContext->SetName(L"main");

	sPerformance  = std::make_unique<Performance>();
	sCpuTimestamp = std::make_unique<CpuTimestamp>();

	sAsyncThreadCollection = std::make_unique<AsyncThreadCollection>();
	sAsyncThreadCollection->Init();

	sWindowCollection = std::make_unique<WindowCollection>();
	sInput            = std::make_unique<Input>();
	sImGuiController  = std::make_unique<ImGuiController>();
}

void System::Term() {
	WinApp::Term();
}

void System::Shutdown() {
	sInput->Shutdown();
	sAsyncThreadCollection->SetTerminate();
	sAsyncThreadCollection->Shutdown();
}

DXOBJECT Descriptor System::GetDescriptor(DXOBJECT DescriptorType type) {
	return sDirectXCommon->GetDesriptorHeaps()->GetDescriptor(type);
}

DXOBJECT Device* System::GetDxDevice() {
	return sDirectXCommon->GetDevice();
}

DXOBJECT DescriptorHeaps* System::GetDxDescriptorHeaps() {
	return sDirectXCommon->GetDesriptorHeaps();
}

void System::TransitionAllocator() {
	sDirectQueueContext->TransitionAllocator();
}

void System::ExecuteAllAllocator() {
	sDirectQueueContext->ExecuteAllAllocators();
}

DirectXQueueContext* System::GetDirectQueueContext() {
	return sDirectQueueContext.get();
}

const std::weak_ptr<DirectXWindowContext> System::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	// windowの生成
	auto window = sWindowCollection->CreateMainWindow(clientSize, name, clearColor);

	// user system の初期化
	sInput->Init(window.lock().get());
	sImGuiController->Init(window.lock().get());

	return window;
}

const std::weak_ptr<DirectXWindowContext> System::CreateSubWindow(const Vector2ui& clientSize, const LPCWSTR& name, DirectXWindowContext::ProcessCategory category, const Color4f& clearColor) {
	return sWindowCollection->CreateSubWindow(clientSize, name, category, clearColor);
}

bool System::ProcessMessage() {
	return sWindowCollection->ProcessMessage();
}

void System::PresentWindows() {
	sWindowCollection->PresentWindows();
}

void System::RemoveClosedWindow() {
	sWindowCollection->RemoveClosedWindow();
}

DirectXWindowContext* System::GetMainWindow() {
	return sWindowCollection->GetMainWindow();
}

DirectXWindowContext* System::GetFocusWindow() {
	return sWindowCollection->GetFocusWindow();
}

WindowCollection* System::GetWindowCollection() {
	return sWindowCollection.get();
}

bool System::IsPressKey(KeyId id) {
	return sInput->IsPressKey(id);
}

bool System::IsTriggerKey(KeyId id) {
	return sInput->IsTriggerKey(id);
}

bool System::IsReleaseKey(KeyId id) {
	return sInput->IsReleaseKey(id);
}

const KeyboardInput* System::GetKeyboardInput() {
	return sInput->GetKeyboardInput();
}

const MouseInput* System::GetMouseInput() {
	return sInput->GetMouseInput();
}

const GamepadInput* System::GetGamepadInput(uint8_t number) {
	return sInput->GetGamepadInput(number);
}

Input* System::GetInput() {
	return sInput.get();
}

void System::BeginPerformance() {
	sPerformance->Begin();
	sCpuTimestamp->Begin();
}

void System::EndPerformance() {
	sPerformance->End();
	sCpuTimestamp->End();
}

TimePointd<TimeUnit::second> System::GetDeltaTimed() {
	return sPerformance->GetDeltaTimed();
}

TimePointf<TimeUnit::second> System::GetDeltaTimef() {
	return sPerformance->GetDeltaTimef();
}

void System::Record(const std::string& name) {
	sCpuTimestamp->Record(name);
}

Performance* System::GetPerformance() {
	return sPerformance.get();
}

CpuTimestamp* System::GetCpuTimestamp() {
	return sCpuTimestamp.get();
}

void System::PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task) {
	sAsyncThreadCollection->PushTask(execution, task);
}

std::shared_ptr<AsyncTask> System::PushTask(AsyncExecution execution, const AsyncTask::Function& function) {
	std::shared_ptr<AsyncTask> task = std::make_shared<AsyncTask>();
	task->SetFunction(function);
	task->SetTag("function task");

	sAsyncThreadCollection->PushTask(execution, task);

	return task;
}

AsyncThreadCollection* System::GetAsyncThreadCollection() {
	return sAsyncThreadCollection.get();
}

void System::BeginImGuiFrame() {
	sImGuiController->BeginFrame();
}

void System::EndImGuiFrame() {
	sImGuiController->EndFrame();
}

void System::RenderImGui(DirectXQueueContext* context) {
	sImGuiController->Render(context);
}

ImGuiController* System::GetImGuiController() {
	return sImGuiController.get();
}
