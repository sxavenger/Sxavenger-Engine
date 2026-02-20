#include "DirectXWindowContext.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <windows.h>

//* external
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

std::wstring DirectXWindowContext::Parameter::GetWindowClassName() const {
	return L"[Sxavenger Engine Window]: " + name;
}

RECT DirectXWindowContext::Parameter::GetWindowRect() const {
	RECT rect = {};
	rect.right  = static_cast<LONG>(client.x);
	rect.bottom = static_cast<LONG>(client.y);
	return rect;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXWindowContext::Window::Create(const Parameter& parameter, Sxl::Flag<Style> flag) {

	//!< インスタンスの生成
	hinst = GetModuleHandle(nullptr);

	//!< ウィンドウクラス名の設定
	className = parameter.GetWindowClassName();

	//!< window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className.c_str();
	wc.hInstance     = hinst;
	wc.lpfnWndProc   = GetWindowProcFunction(parameter.category);
	StreamLogger::AssertA(RegisterClass(&wc), "window register class failed.");

	//!< rectの設定
	rect = parameter.GetWindowRect();

	//!< windowスタイルの設定
	style = flag;

	AdjustWindowRect(&rect, style.Get(), false); //!< ウィンドウサイズの調整

	//!< ウィンドウを生成
	hwnd = CreateWindow(
		wc.lpszClassName,
		parameter.name.c_str(),
		style.Get(),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hinst,
		this
	);
	StreamLogger::AssertA(hwnd != nullptr, "window create failed.");

	StreamLogger::EngineLog(std::format(L"[DirectXWindowContext]: create window. name: {}, hwnd: {:p}", className, static_cast<const void*>(hwnd)));
}

void DirectXWindowContext::Window::Show(DWORD flag) const {
	ShowWindow(hwnd, flag);
}

void DirectXWindowContext::Window::Close() {
	if (hwnd == nullptr) {
		return;
	}

	DestroyWindow(hwnd);
	CloseWindow(hwnd);

	if (hinst != nullptr) {
		UnregisterClass(className.c_str(), hinst);
	}

	StreamLogger::EngineLog(std::format(L"[DirectXWindowContext]: close window. name: {}, hwnd: {:p}", className, static_cast<const void*>(hwnd)));
}

void DirectXWindowContext::Window::SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const {
	HICON icon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, iconSize.x, iconSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}

void DirectXWindowContext::Window::SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const {
	HICON icon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, iconSize.x, iconSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
}

void DirectXWindowContext::Window::SetMode(Mode _mode) {
	if (mode == _mode) {
		return; //!< すでに同じモードの場合は処理を行わない
	}

	mode = _mode; //!< モードの更新

	switch (mode) {
		case Mode::Borderless:
			{
				rect = GetCurrentRect(hwnd); //!< 現在のウィンドウサイズを保存

				//!< ボーダーレスフルスクリーンに変更
				DWORD flag
					= style.Get()
					& ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME); //!< タイトルバーとサイズ変更可能なスタイルを削除

				//!< 仮想フルスクリーン化
				SetWindowLong(
					hwnd,
					GWL_STYLE,
					flag
				);

				//!< モニター情報の取得
				HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

				MONITORINFO info = { sizeof(info) };
				GetMonitorInfo(monitor, &info);

				RECT screen = {};
				screen.right  = info.rcMonitor.right - info.rcMonitor.left;
				screen.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

				//!< ウィンドウの位置を変更
				SetWindowPos(
					hwnd,
					HWND_TOPMOST,
					screen.left,
					screen.top,
					screen.right,
					screen.bottom,
					SWP_NOACTIVATE
				);

				//!< ウィンドウの表示
				Show(SW_MAXIMIZE);

				// TODO: マウスの補正処理を追加する
			}
			break;

		case Mode::Window:
			{
				//!< 通常windowに変更
				SetWindowLong(hwnd, GWL_STYLE, style.Get());

				RECT screen = {};
				screen.left   = rect.left;
				screen.top    = rect.top;
				screen.right  = rect.right - rect.left;
				screen.bottom = rect.bottom - rect.top;

				//!< ウィンドウサイズの調整
				SetWindowPos(
					hwnd,
					HWND_NOTOPMOST,
					screen.left,
					screen.top,
					screen.right,
					screen.bottom,
					SWP_FRAMECHANGED | SWP_NOACTIVATE
				);

				//!< ウィンドウの表示
				Show(SW_NORMAL);
			}
			break;
	}
}

RECT DirectXWindowContext::Window::GetCurrentRect(HWND hwnd) {
	RECT rect = {};
	GetWindowRect(hwnd, &rect);

	return rect;
}

LRESULT DirectXWindowContext::Window::WindowProcApplication(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	auto instance = reinterpret_cast<DirectXWindowContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
		case WM_SYSCHAR:
			return 0;

		case WM_CREATE:
			{
				auto structure = reinterpret_cast<CREATESTRUCT*>(lparam);
				auto app = reinterpret_cast<LONG_PTR>(structure->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, app);
			}
			break;

		case WM_DESTROY: //!< ウィンドウが破棄された
			// OSに対して, アプリの終了を伝える
			PostQuitMessage(0);
			return 0;

		case WM_MOVE:          //!< windowが移動した
		case WM_DISPLAYCHANGE: //!< displayの設定が変更された
			//instance->CheckSupportHDR();
			break;

		case WM_SIZE: //!< windowのサイズが変更された
			{
				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化されたときはリサイズ処理を行わない
				}

				// 新しいサイズを取得
				Vector2ui size = { LOWORD(lparam), HIWORD(lparam) };

				if (instance != nullptr) {
					instance->ResizeWindow(size);
				}
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT DirectXWindowContext::Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	auto instance = reinterpret_cast<DirectXWindowContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
		case WM_SYSCHAR:
			return 0;

		case WM_CLOSE: //!< windowが終了した
			// windowの破棄
			DestroyWindow(hwnd);
			return 0;

		case WM_MOVE:          //!< windowが移動した
		case WM_DISPLAYCHANGE: //!< displayの設定が変更された
			//instance->CheckSupportHDR();
			break;

		case WM_SIZE: //!< windowのサイズが変更された
			{
				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化されたときはリサイズ処理を行わない
				}

				// 新しいサイズを取得
				Vector2ui size = { LOWORD(lparam), HIWORD(lparam) };

				if (instance != nullptr) {
					instance->ResizeWindow(size);
				}
			}
			break;

	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WNDPROC DirectXWindowContext::Window::GetWindowProcFunction(ProcessCategory category) {
	switch (category) {
		case ProcessCategory::Application:
			return WindowProcApplication;

		case ProcessCategory::Window:
			return WindowProc;

		default:
			StreamLogger::Exception("category is not valid.");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXWindowContext::Init(
	const Vector2ui& client, const std::wstring& name,
	ProcessCategory category, Sxl::Flag<Style> style,
	const Color4f& color) {

	parameter_ = {};
	parameter_.client     = client;   //!< クライアント領域のサイズ
	parameter_.name       = name;     //!< ウィンドウの名前
	parameter_.category   = category; //!< ウィンドウのプロセスカテゴリ
	parameter_.clearColor = color;    //!< ウィンドウのクリアカラー

	window_.Create(parameter_, style); //!< windowの設定
	SetWindowLongPtr(window_.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); //!< instanceの設定

	//!< swapchainの生成
	swapChain_ = std::make_unique<DxObject::SwapChain>();
	swapChain_->Init(
		System::GetDxDevice(), System::GetDxDescriptorHeaps(), System::GetDirectQueueContext()->GetDxCommand(),
		DxObject::kDefaultScreenFormat, parameter_.client, window_.hwnd
	);

	// depth stencilの生成
	depthStencil_ = std::make_unique<DxObject::DepthStencil>();
	depthStencil_->Init(System::GetDxDevice(), System::GetDxDescriptorHeaps(), parameter_.client);

	window_.Show(SW_SHOW); //!< windowの表示
}

void DirectXWindowContext::Close() {
	window_.Close();
}

void DirectXWindowContext::SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const {
	window_.SetWindowIcon(filepath, iconSize);
}

void DirectXWindowContext::SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const {
	window_.SetTaskbarIcon(filepath, iconSize);
}

void DirectXWindowContext::SetIcon(const std::filesystem::path& filepath, const Vector2ui& iconSize) const {
	window_.SetWindowIcon(filepath, iconSize);
	window_.SetTaskbarIcon(filepath, iconSize);
}

void DirectXWindowContext::SetWindowMode(Mode mode) {
	window_.SetMode(mode);
}

void DirectXWindowContext::ResizeWindow(const Vector2ui& client) {

	swapChain_->Resize(System::GetDxDevice(), System::GetDxDescriptorHeaps(), DxObject::kDefaultScreenFormat, client);
	depthStencil_->Init(System::GetDxDevice(), System::GetDxDescriptorHeaps(), client);

	//!< ウィンドウサイズの保存
	parameter_.client = client;
}

void DirectXWindowContext::BeginRenderWindow(const DirectXQueueContext* context) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = context->GetCommandList();

	// backBufferを書き込み状態に変更
	D3D12_RESOURCE_BARRIER barrier = swapChain_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	// RTV, DSVの設定
	commandList->OMSetRenderTargets(
		1, &swapChain_->GetBackBufferCPUHandle(),
		false,
		&depthStencil_->GetCPUHandle()
	);

}

void DirectXWindowContext::EndRenderWindow(const DirectXQueueContext* context) {
	// backBufferをpresent状態に変更
	D3D12_RESOURCE_BARRIER barrier = swapChain_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void DirectXWindowContext::ClearWindow(const DirectXQueueContext* context) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = context->GetCommandList();

	// 画面のクリア
	commandList->ClearRenderTargetView(
		swapChain_->GetBackBufferCPUHandle(),
		&parameter_.clearColor.r,
		0, nullptr
	);

	// 深度をクリア
	commandList->ClearDepthStencilView(
		depthStencil_->GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DirectXWindowContext::Present() {
	swapChain_->Present();
}

const DxObject::SwapChain::ColorSpace DirectXWindowContext::GetColorSpace() const {
	StreamLogger::AssertA(swapChain_ != nullptr, "window is not created.");
	return swapChain_->GetColorSpace();
}

int32_t DirectXWindowContext::ComputeIntersectionArea(const RECT& rectA, const RECT& rectB) {
	return std::max<LONG>(0, std::min(rectA.right, rectB.right) - std::max(rectA.left, rectB.left))
		* std::max<LONG>(0, std::min(rectA.bottom, rectB.bottom) - std::max(rectA.top, rectB.top));
}

ComPtr<IDXGIOutput6> DirectXWindowContext::GetOutput6() {

	// rectの取得
	GetWindowRect(window_.hwnd, &window_.rect);

	ComPtr<IDXGIOutput> output;
	std::optional<int32_t> outputArea = std::nullopt;

	// 各displayの確認
	ComPtr<IDXGIOutput> current;

	for (UINT i = 0;
		System::GetDxDevice()->GetAdapter()->EnumOutputs(i, &current) != DXGI_ERROR_NOT_FOUND;
		++i) {

		// displayの情報を取得
		DXGI_OUTPUT_DESC desc = {};
		auto hr = current->GetDesc(&desc);
		DxObject::Assert(hr, L"DXGI_OUTPUT_DESC GetDesc() failed.");

		int32_t currentArea = ComputeIntersectionArea(window_.rect, desc.DesktopCoordinates);

		// displayの位置がウィンドウの位置と重なっているか確認
		if (currentArea > outputArea.value_or(-1)) {
			output     = current;
			outputArea = currentArea;
		}
	}

	ComPtr<IDXGIOutput6> output6;
	auto hr = output.As(&output6);
	DxObject::Assert(hr, L"IDXGIOutput6 QueryInterface() failed.");

	return output6;
}

void DirectXWindowContext::CheckSupportHDR() {

	if (swapChain_ == nullptr) {
		RuntimeLogger::LogWarning("[DirectXWindowContext]", "window is not create.");
		return; //!< windowが生成されていない.
	}

	ComPtr<IDXGIOutput6> output6 = GetOutput6();

	if (output6 == nullptr) {
		return;
	}

	DXGI_OUTPUT_DESC1 desc = {};
	auto hr = output6->GetDesc1(&desc);
	DxObject::Assert(hr, L"IDXGIOutput6 GetDesc1() failed.");

	swapChain_->SetColorSpace(desc);
}
