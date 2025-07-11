#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "Config/SxavengerConfig.h"
#include "WinApp/WinApp.h"
#include "DirectX/DirectXCommon.h"
#include "DirectX/DirectXContext.h"
#include "Window/GameWindowCollection.h"
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

	//-----------------------------------------------------------------------------------------
	// DirectXCommon option
	//-----------------------------------------------------------------------------------------

	_NODISCARD static _DXOBJECT Descriptor GetDescriptor(_DXOBJECT DescriptorType type);

	static _DXOBJECT Device* GetDxDevice();

	static _DXOBJECT DescriptorHeaps* GetDxDescriptorHeaps();

	//-----------------------------------------------------------------------------------------
	// DirectXThreadContext main thread option
	//-----------------------------------------------------------------------------------------

	static void TransitionAllocator();

	static void ExecuteAllAllocator();

	static DirectXThreadContext* GetMainThreadContext();

	//-----------------------------------------------------------------------------------------
	// GameWindowCollection option
	//-----------------------------------------------------------------------------------------

	static const std::weak_ptr<GameWindow> CreateMainWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	static const std::weak_ptr<GameWindow> TryCreateSubWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	static bool ProcessMessage();

	static void PresentAllWindow();

	static const std::weak_ptr<GameWindow> GetMainWindow();

	static const GameWindow* GetForcusWindow();

	static GameWindowCollection* GetGameWindowCollection();

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

	static TimePointf<TimeUnit::second> GetDeltaTime();

	static void RecordLap(const std::string& name);

	static Performance* GetPerformance();

	//-----------------------------------------------------------------------------------------
	// Async thread collection option
	//-----------------------------------------------------------------------------------------

	static void PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task);

	static void PushTaskAndWait(AsyncExecution execution, const AsyncTask::Function& function);

	static void ShutdownAsyncThread();

	//-----------------------------------------------------------------------------------------
	// imgui controller option
	//-----------------------------------------------------------------------------------------

	static void BeginImGuiFrame();

	static void EndImGuiFrame();

	static void RenderImGui(DirectXThreadContext* context = GetMainThreadContext());

	static ImGuiController* GetImGuiController();

private:
};
