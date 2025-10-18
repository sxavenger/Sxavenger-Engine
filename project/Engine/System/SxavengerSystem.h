#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "Config/SxavengerConfig.h"
#include "WinApp/WinApp.h"
#include "DirectX/DirectXCommon.h"
#include "DirectX/Context/DirectXQueueContext.h"
//#include "DirectX/DirectXContext.h"
#include "Window/WindowCollection.h"
#include "Runtime/Input/Input.h"
#include "Runtime/Performance/Performance.h"
#include "Runtime/Thread/AsyncThreadCollection.h"
#include "UI/ImGuiController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerSystem class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerSystem {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	static void Shutdown();

	//-----------------------------------------------------------------------------------------
	// DirectXCommon option
	//-----------------------------------------------------------------------------------------

	_NODISCARD static _DXOBJECT Descriptor GetDescriptor(_DXOBJECT DescriptorType type);

	static _DXOBJECT Device* GetDxDevice();

	static _DXOBJECT DescriptorHeaps* GetDxDescriptorHeaps();

	//-----------------------------------------------------------------------------------------
	// DirectXQueueContext main thread option
	//-----------------------------------------------------------------------------------------

	static void TransitionAllocator();

	static void ExecuteAllAllocator();

	static DirectXQueueContext* GetDirectQueueContext();

	//-----------------------------------------------------------------------------------------
	// GameWindowCollection option
	//-----------------------------------------------------------------------------------------

	static const std::weak_ptr<DirectXWindowContext> CreateMainWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = DirectXWindowContext::kDefaultClearColor
	);

	static const std::weak_ptr<DirectXWindowContext> CreateSubWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, DirectXWindowContext::ProcessCategory category, const Color4f& clearColor = DirectXWindowContext::kDefaultClearColor
	);

	static bool ProcessMessage();

	static void PresentWindows();

	static DirectXWindowContext* GetMainWindow();

	static DirectXWindowContext* GetForcusWindow();

	static WindowCollection* GetWindowCollection();

	//-----------------------------------------------------------------------------------------
	// Input option
	//-----------------------------------------------------------------------------------------

	static bool IsPressKey(KeyId id);

	static bool IsTriggerKey(KeyId id);

	static bool IsReleaseKey(KeyId id);

	static Input* GetInput();

	//-----------------------------------------------------------------------------------------
	// Performance option
	//-----------------------------------------------------------------------------------------

	static void BeginPerformace();

	static void EndPerformace();

	static TimePointd<TimeUnit::second> GetDeltaTime();
	static TimePointf<TimeUnit::second> GetDeltaTimef();

	static void RecordLap(const std::string& name);

	static Performance* GetPerformance();

	//-----------------------------------------------------------------------------------------
	// Async thread collection option
	//-----------------------------------------------------------------------------------------

	static void PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task);

	static void PushTaskAndWait(AsyncExecution execution, const AsyncTask::Function& function);

	static AsyncThreadCollection* GetAsyncThreadCollection();

	//-----------------------------------------------------------------------------------------
	// imgui controller option
	//-----------------------------------------------------------------------------------------

	static void BeginImGuiFrame();

	static void EndImGuiFrame();

	static void RenderImGui(DirectXQueueContext* context = GetDirectQueueContext());

	static ImGuiController* GetImGuiController();

private:
};
