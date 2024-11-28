#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "WinApp/WinApp.h"
#include "DirectX/DirectXCommon.h"
#include "DirectX/DirectXContext.h"
#include "Window/GameWindowCollection.h"
#include "Runtime/Thread/Thread.h"
#include "Runtime/Input/Input.h"
#include "UI/ImGuiController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerSystemEngine class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerSystemEngine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// DirectXCommon option
	//-----------------------------------------------------------------------------------------

	static _DXOBJECT Descriptor GetDescriptor(_DXOBJECT DescriptorType type);

	static _DXOBJECT Device* GetDxDevice();

	static _DXOBJECT DescriptorHeaps* GetDxDescriptorHeaps();

	static _DXOBJECT CompileBlobCollection* GetDxCompileBlobCollection();

	//-----------------------------------------------------------------------------------------
	// DirectXThreadContext main thread option
	//-----------------------------------------------------------------------------------------

	static void TransitionAllocator();

	static void ExecuteAllAllocator();

	static ID3D12GraphicsCommandList6* GetCommandList();

	static DirectXThreadContext* GetMainThreadContext();

	//-----------------------------------------------------------------------------------------
	// ThreadCollection option
	//-----------------------------------------------------------------------------------------

	static void PushTask(TaskThreadExecution* thread);

	static ThreadCollection* GetThreadCollection();

	//-----------------------------------------------------------------------------------------
	// GameWindowCollection option
	//-----------------------------------------------------------------------------------------

	static GameWindow* CreateMainWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	static const std::weak_ptr<GameWindow> TryCreateSubWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	static bool ProcessMessage();

	static void PresentAllWindow();

	static const GameWindow* GetMainWindow();

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
	// imgui controller option
	//-----------------------------------------------------------------------------------------

	static void BeginImGuiFrame();

	static void EndImGuiFrame();

	static void RenderImGui(DirectXThreadContext* context = GetMainThreadContext());

	static ImGuiController* GetImGuiController();

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using SxavengerSystem = SxavengerSystemEngine;