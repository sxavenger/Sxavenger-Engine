#include "DirectXWindowContext.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <windows.h>

//* external
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXWindowContext::Init(const Vector2ui size, const std::wstring& name, ProcessCategory category, const Color4f& color) {

	// 引数の保存
	size_        = size;
	name_        = name;
	clearColor_  = color;
	category_    = category;

	InitWindow();
	InitDirectXWindow();

	Logger::EngineLog(std::format(L"[DirectXWindowContext]: show window. name: {}, hwnd: {:p}", name_, static_cast<const void*>(hwnd_)));
}

void DirectXWindowContext::Close() {
	if (hwnd_ == nullptr) {
		return;
	}

	DestroyWindow(hwnd_);
	CloseWindow(hwnd_);
	Logger::EngineLog(std::format(L"[DirectXWindowContext]: close window. name: {}, hwnd: {:p}", name_, static_cast<const void*>(hwnd_)));

	if (hinst_ != nullptr) {
		UnregisterClass(className_.c_str(), hinst_);
	}
}

void DirectXWindowContext::SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const {
	HICON smallIcon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, cursolSize.x, cursolSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon));
}

void DirectXWindowContext::SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const {
	HICON largeIcon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, cursolSize.x, cursolSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(largeIcon));
}

void DirectXWindowContext::SetIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const {
	SetWindowIcon(filepath, cursolSize);
	SetTaskbarIcon(filepath, cursolSize);
}

void DirectXWindowContext::SetWindowMode(Mode mode) {
	if (mode_ == mode) {
		return; //!< 既に設定されている
	}

	mode_ = mode;

	uint32_t style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	// todo: styleを指定できるように変更

	switch (mode) {
		case Mode::Borderless:
			{
				// 元の状態を取得
				GetWindowRect(hwnd_, &rect_);

				// 仮想フルスクリーン化
				SetWindowLong(
					hwnd_,
					GWL_STYLE,
					style & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
				);

				// モニター情報の取得
				HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
				MONITORINFO info = {};
				info.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(monitor, &info);

				RECT rc = {};
				rc.right  = info.rcMonitor.right - info.rcMonitor.left;
				rc.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

				// ウィンドウの位置を変更
				SetWindowPos(
					hwnd_, HWND_TOPMOST,
					rc.left, rc.top, rc.right, rc.bottom,
					SWP_NOACTIVATE
				);

				ShowWindow(hwnd_, SW_MAXIMIZE);
			}
			break;

		case Mode::Window:
			{
				// 通常ウィンドウに戻す
				SetWindowLong(hwnd_, GWL_STYLE, style);

				// ウィンドウの位置を変更
				RECT rc = {};
				rc.left   = rect_.left;
				rc.top    = rect_.top;
				rc.right  = rect_.right - rect_.left;
				rc.bottom = rect_.bottom - rect_.top;

				SetWindowPos(
					hwnd_, HWND_NOTOPMOST,
					rc.left, rc.top, rc.right, rc.bottom,
					SWP_FRAMECHANGED | SWP_NOACTIVATE
				);

				ShowWindow(hwnd_, SW_NORMAL);
			}
			break;
	}
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
		&clearColor_.r,
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
	Exception::Assert(swapChain_ != nullptr, "window is not created.");
	return swapChain_->GetColorSpace();
}

LRESULT DirectXWindowContext::WindowProcApplication(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	auto instance = reinterpret_cast<DirectXWindowContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
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
			instance->CheckSupportHDR();
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT DirectXWindowContext::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	//auto instance = reinterpret_cast<DirectXWindowContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
		case WM_CLOSE: //!< windowが終了した
			// windowの破棄
			DestroyWindow(hwnd);
			return 0;

		case WM_MOVE:          //!< windowが移動した
		case WM_DISPLAYCHANGE: //!< displayの設定が変更された
			//instance->CheckSupportHDR();
			break;

	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int32_t DirectXWindowContext::ComputeIntersectionArea(const RECT& rectA, const RECT& rectB) {
	return std::max<LONG>(0, std::min(rectA.right, rectB.right) - std::max(rectA.left, rectB.left))
		* std::max<LONG>(0, std::min(rectA.bottom, rectB.bottom) - std::max(rectA.top, rectB.top));
}

WNDPROC DirectXWindowContext::GetWindowProc() const {
	switch (category_) {
		case ProcessCategory::Application:
			return WindowProcApplication;

		case ProcessCategory::Window:
			return WindowProc;

		default:
			return nullptr; //!< 未定義のcategory
	}
}

ComPtr<IDXGIOutput6> DirectXWindowContext::GetOutput6() {

	// rectの取得
	GetWindowRect(hwnd_, &rect_);

	ComPtr<IDXGIOutput> output;
	std::optional<int32_t> outputArea = std::nullopt;

	// 各displayの確認
	ComPtr<IDXGIOutput> current;

	for (UINT i = 0;
		SxavengerSystem::GetDxDevice()->GetAdapter()->EnumOutputs(i, &current) != DXGI_ERROR_NOT_FOUND;
		++i) {

		// displayの情報を取得
		DXGI_OUTPUT_DESC desc = {};
		auto hr = current->GetDesc(&desc);
		Exception::Assert(SUCCEEDED(hr), "DXGI_OUTPUT_DESC GetDesc() failed.");

		int32_t currentArea = ComputeIntersectionArea(rect_, desc.DesktopCoordinates);

		// displayの位置がウィンドウの位置と重なっているか確認
		if (currentArea > outputArea.value_or(-1)) {
			output     = current;
			outputArea = currentArea;
		}
	}

	ComPtr<IDXGIOutput6> output6;
	auto hr = output.As(&output6);
	Exception::Assert(SUCCEEDED(hr), "IDXGIOutput6 QueryInterface() failed.");

	return output6;
}

void DirectXWindowContext::CheckSupportHDR() {

	if (swapChain_ == nullptr) {
		Logger::WarningRuntime("[DirectXWindowContext] warning | window is not create.");
		return; //!< windowが生成されていない.
	}

	ComPtr<IDXGIOutput6> output6 = GetOutput6();

	if (output6 == nullptr) {
		return;
	}

	DXGI_OUTPUT_DESC1 desc = {};
	auto hr = output6->GetDesc1(&desc);
	Exception::Assert(SUCCEEDED(hr), "IDXGIOutput6 GetDesc1() failed.");

	swapChain_->SetColorSpace(desc);
}

void DirectXWindowContext::InitWindow() {

	// インスタンスハンドルを取得
	hinst_ = GetModuleHandle(nullptr);
	Exception::Assert(hinst_ != nullptr);

	// 引数の保存
	className_ = L"Sxavenger Engine Window: " + name_;

	// window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className_.c_str();
	wc.hInstance     = hinst_;
	wc.lpfnWndProc   = GetWindowProc();
	Exception::Assert(RegisterClass(&wc));

	rect_ = {};
	rect_.right  = size_.x;
	rect_.bottom = size_.y;

	// ウィンドウサイズの調整
	AdjustWindowRect(&rect_, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウを生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		name_.c_str(),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //!< windowのサイズの固定
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect_.right - rect_.left,
		rect_.bottom - rect_.top,
		nullptr,
		nullptr,
		hinst_,
		this
	);
	Exception::Assert(hwnd_ != nullptr);

	// ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);

	// ウィンドウのプロシージャを設定
	//SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void DirectXWindowContext::InitDirectXWindow() {

	// swapchainの生成
	swapChain_ = std::make_unique<DxObject::SwapChain>();
	swapChain_->Init(
		SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), SxavengerSystem::GetDirectQueueContext()->GetDxCommand(),
		DxObject::kDefaultScreenFormat, size_, hwnd_
	);

	// depth stencilの生成
	depthStencil_ = std::make_unique<DxObject::DepthStencil>();
	depthStencil_->Init(SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), size_);

}
