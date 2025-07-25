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
#include <Engine/System/UI/ISystemDebugGui.h>

//* windows
#include <windef.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Color4.h>

//* c++
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class
////////////////////////////////////////////////////////////////////////////////////////////
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
		Borderless,
		Window
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXWindowContext()  = default;
	~DirectXWindowContext() { Close(); }

	void Init(const Vector2ui size, const std::wstring& name, ProcessCategory category = ProcessCategory::Application, const Color4f& color = kDefaultClearColor);

	void Close();

	//* icon option *//

	void SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const;

	void SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const;

	void SetIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) const;

	//* mode option *//

	void SetWindowMode(Mode mode);

	//* directx option *//

	void BeginRenderWindow(const DirectXQueueContext* context);

	void EndRenderWindow(const DirectXQueueContext* context);

	void ClearWindow(const DirectXQueueContext* context);

	void Present();

	//* parameter getter *//

	const std::wstring& GetName() const { return name_; }

	const Vector2ui& GetSize() const { return size_; }

	ProcessCategory GetCategory() const { return category_; }

	//* window getter *//

	const HINSTANCE& GetHinst() const { return hinst_; }

	const HWND& GetHwnd() const { return hwnd_; }

	bool IsOpenWindow() const { return hwnd_ != nullptr && IsWindow(hwnd_); }

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

	//* parameter *//

	Vector2ui size_ = {};
	std::wstring name_;

	ProcessCategory category_;

	Color4f clearColor_;

	//* windows *//

	HINSTANCE hinst_;
	HWND      hwnd_;

	std::wstring className_;

	RECT rect_ = {};

	Mode mode_ = Mode::Window;

	//* DirectX *//

	std::unique_ptr<DxObject::SwapChain>    swapChain_;
	std::unique_ptr<DxObject::DepthStencil> depthStencil_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window proc methods *//

	static LRESULT CALLBACK WindowProcApplication(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//* helper methods *//

	static int32_t ComputeIntersectionArea(const RECT& rectA, const RECT& rectB);

	WNDPROC GetWindowProc() const;

	ComPtr<IDXGIOutput6> GetOutput6();

	void CheckSupportHDR();

	//* initialization methods *//

	void InitWindow();

	void InitDirectXWindow();

};
