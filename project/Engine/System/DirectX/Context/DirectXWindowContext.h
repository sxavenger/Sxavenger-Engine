#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* context
#include "DirectXQueueContext.h"

//* DXOBJECT
#include "../DxObject/DxSwapChain.h"
#include "../DxObject/DxDepthStencil.h"

//* engine
#include <Engine/Foundation.h>

//* windows
#include <windef.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Color4.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <string>
#include <memory>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DirectXを使用したウィンドウの作成, 管理, 描画処理を行うクラス.
class DirectXWindowContext {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessCategory enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ProcessCategory : uint8_t {
		Application,
		Window,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		//Fullscreen, //!< フルスクリーン
		Borderless, //!< ボーダーレスフルスクリーン
		Window      //!< ウィンドウモード
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Style enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Style : DWORD {
		Overlapped = WS_OVERLAPPED, //!< 通常windowスタイル.
		Popup      = WS_POPUP,      //!< フルスクリーンスタイル.
		Child      = WS_CHILD,      //!< 子windowスタイル.

		Titlebar   = WS_CAPTION, //!< タイトルバーを表示するスタイル.
		SystemMenu = WS_SYSMENU, //!< システムメニューを表示するスタイル.

		Resizeable = WS_THICKFRAME, //!< サイズ変更可能なスタイル.

		MinimizeBox = WS_MINIMIZEBOX, //!< 最小化ボタンを表示するスタイル.
		MaximizeBox = WS_MAXIMIZEBOX, //!< 最大化ボタンを表示するスタイル.

		Default = Overlapped | Titlebar | SystemMenu | Resizeable | MinimizeBox | MaximizeBox //!< デフォルトのスタイル.
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::wstring GetWindowClassName() const;

		RECT GetWindowRect() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui client  = {};
		std::wstring name = L"";

		ProcessCategory category;

		Color4f clearColor = kDefaultClearColor;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Window structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Window {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		void Create(const Parameter& parameter, Sxl::Flag<Style> flag);

		void Show(DWORD flag) const;

		void Close();

		void SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const;

		void SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const;

		void SetMode(Mode _mode);

		bool IsOpen() const { return hwnd != nullptr && IsWindow(hwnd); }

		static RECT GetCurrentRect(HWND hwnd);

		static LRESULT CALLBACK WindowProcApplication(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		static WNDPROC GetWindowProcFunction(ProcessCategory category);

		//=========================================================================================
		// public variables
		//=========================================================================================

		HINSTANCE hinst;
		HWND      hwnd;

		std::wstring className;

		RECT rect = {};

		Mode mode = Mode::Window;

		Sxl::Flag<Style> style = Style::Default;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXWindowContext()  = default;
	~DirectXWindowContext() { Close(); }

	void Init(
		const Vector2ui& client, const std::wstring& name,
		ProcessCategory category = ProcessCategory::Application, Sxl::Flag<Style> style = Style::Default,
		const Color4f& color = kDefaultClearColor
	);

	//! @brief ウィンドウを閉じる
	void Close();

	//* icon option *//

	//! @brief ウィンドウアイコンを設定する
	void SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const;

	//! @brief タスクバーアイコンを設定する
	void SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const;

	//! @brief ウィンドウとタスクバーのアイコンを設定する
	void SetIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const;

	//* window option *//

	//! @brief ウィンドウモードを設定する
	void SetWindowMode(Mode mode);

	//! @brief ウィンドウのサイズを変更する
	//! @param size 新しいウィンドウのサイズ
	void ResizeWindow(const Vector2ui& client);

	//* DirectX option *//

	//! @brief Window描画開始処理
	void BeginRenderWindow(const DirectXQueueContext* context);

	//! @brief Window描画終了処理
	void EndRenderWindow(const DirectXQueueContext* context);

	//! @brief Window描画クリア処理
	void ClearWindow(const DirectXQueueContext* context);

	//! @brief バックバッファの表示処理
	void Present();

	//* parameter getter *//

	const std::wstring& GetName() const { return parameter_.name; }

	const Vector2ui& GetClient() const { return parameter_.client; }

	ProcessCategory GetCategory() const { return parameter_.category; }

	//* window getter *//

	const HINSTANCE& GetHinst() const { return window_.hinst; }

	const HWND& GetHwnd() const { return window_.hwnd; }

	//! @brief ウィンドウが開かれているか確認する
	//! @retval true  ウィンドウが開かれている
	//! @retval false ウィンドウが閉じられている
	bool IsOpenWindow() const { return window_.IsOpen(); }

	//* DirectX getter *//

	const DxObject::SwapChain::ColorSpace GetColorSpace() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static inline const Color4f kDefaultClearColor = Color4f::Convert(0x9BA8A8FF);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Parameter parameter_;

	Window window_;

	//* DirectX *//

	std::unique_ptr<DxObject::SwapChain>    swapChain_;
	std::unique_ptr<DxObject::DepthStencil> depthStencil_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper hdr methods *//

	static int32_t ComputeIntersectionArea(const RECT& rectA, const RECT& rectB);

	ComPtr<IDXGIOutput6> GetOutput6();

	void CheckSupportHDR();

};

SXAVENGER_ENGINE_NAMESPACE_END
