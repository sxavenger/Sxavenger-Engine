#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* system
#include "WinApp/WinApp.h"
#include "Configuration/Configuration.h"
#include "Utility/StreamLogger.h"
#include "Utility/RuntimeLogger.h"
#include "DirectX/DirectXCommon.h"
#include "DirectX/Context/DirectXQueueContext.h"
#include "DirectX/Context/DirectXWindowContext.h"
#include "Window/WindowCollection.h"
#include "Runtime/Input/Input.h"
#include "Runtime/Performance/Performance.h"
#include "Runtime/Performance/TimestampCpu.h"
#include "Runtime/Performance/TimestampGpu.h"

#include "Runtime/Thread/AsyncThreadCollection.h"
#include "UI/ImGuiController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// System class
////////////////////////////////////////////////////////////////////////////////////////////
class System final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//! @brief 非同期threadの終了処理
	static void Shutdown();

	//-----------------------------------------------------------------------------------------
	// DirectXCommon option
	//-----------------------------------------------------------------------------------------

	NODISCARD static DXOBJECT Descriptor GetDescriptor(DXOBJECT DescriptorType type);

	static DXOBJECT Device* GetDxDevice();

	static DXOBJECT DescriptorHeaps* GetDxDescriptorHeaps();

	//-----------------------------------------------------------------------------------------
	// DirectXQueueContext main thread option
	//-----------------------------------------------------------------------------------------

	//! @brief DirectQueueのAllocatorを遷移
	static void TransitionAllocator();

	//! @brief DirectQueueの全てのAllocatorを実行
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

	//! @brief メッセージ処理
	//! @retval true  メッセージが存在する
	//! @retval false windowが閉じられた
	static bool ProcessMessage();

	//! @brief 全てのwindowのPresent実行.
	static void PresentWindows();

	//! @brief 閉じられたウィンドウの削除
	static void RemoveClosedWindow();

	//! @brief メインウィンドウの取得
	static DirectXWindowContext* GetMainWindow();

	//! @brief フォーカスされているウィンドウの取得
	static DirectXWindowContext* GetFocusWindow();

	static WindowCollection* GetWindowCollection();

	//-----------------------------------------------------------------------------------------
	// Input option
	//-----------------------------------------------------------------------------------------

	static bool IsPressKey(KeyId id);

	static bool IsTriggerKey(KeyId id);

	static bool IsReleaseKey(KeyId id);

	static const KeyboardInput* GetKeyboardInput();

	static const MouseInput* GetMouseInput();

	static const GamepadInput* GetGamepadInput(uint8_t number);

	static Input* GetInput();

	//-----------------------------------------------------------------------------------------
	// Performance option
	//-----------------------------------------------------------------------------------------

	static void BeginPerformance();

	static void EndPerformance();

	static TimePointd<TimeUnit::second> GetDeltaTimed();
	static TimePointf<TimeUnit::second> GetDeltaTimef();

	static void RecordCpu(const std::string& name);

	static void BeginRecordGpu(const std::string& name);

	static void EndRecordGpu();

	static Performance* GetPerformance();

	static TimestampCpu* GetTimestampCpu();

	static TimestampGpu* GetTimestampGpu();

	//-----------------------------------------------------------------------------------------
	// Async thread collection option
	//-----------------------------------------------------------------------------------------

	//! @brief 非同期タスクを追加
	//! @param[in] execution 実行するスレッド
	//! @param[in] task      実行するタスク
	static void PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task);

	//! @brief 非同期タスクを追加
	//! @param[in] execution 実行するスレッド
	//! @param[in] function 実行する関数
	//! @return 追加されたタスク
	static std::shared_ptr<AsyncTask> PushTask(AsyncExecution execution, const AsyncTask::Function& function);

	static AsyncThreadCollection* GetAsyncThreadCollection();

	//-----------------------------------------------------------------------------------------
	// imgui controller option
	//-----------------------------------------------------------------------------------------

	static void BeginImGuiFrame();

	static void EndImGuiFrame();

	static void RenderImGui(DirectXQueueContext* context = GetDirectQueueContext());

	static ImGuiController* GetImGuiController();

};

SXAVENGER_ENGINE_NAMESPACE_END
