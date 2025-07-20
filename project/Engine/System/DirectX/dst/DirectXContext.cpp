#include "DirectXContext.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXWindowContext::Init(Window* window, DirectXThreadContext* context, const Color4f& clearColor) {

	// 引数の保存
	threadContext_ = context;

	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Init(
		SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), context->GetDxCommand(),
		window
	);

	depthStencil_ = std::make_unique<DepthStencil>();
	depthStencil_->Init(SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), window->GetSize());

	clearColor_ = clearColor;
}

void DirectXWindowContext::Term() {
}

void DirectXWindowContext::BeginRendering() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	swapChain_->GetCurrentBackBufferIndex();

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

void DirectXWindowContext::EndRendering() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	// backBufferをpresent状態に変更
	D3D12_RESOURCE_BARRIER barrier = swapChain_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	commandList->ResourceBarrier(1, &barrier);

}

void DirectXWindowContext::ClearWindow(bool isClearColor, bool isClearDepth) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	if (isClearColor) {
		// 画面のクリア
		commandList->ClearRenderTargetView(
			swapChain_->GetBackBufferCPUHandle(),
			&clearColor_.r,
			0, nullptr
		);
	}

	if (isClearDepth) {
		// 深度をクリア
		commandList->ClearDepthStencilView(
			depthStencil_->GetCPUHandle(),
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0, 0, nullptr
		);
	}

	isClearWindow_ = true;
}

void DirectXWindowContext::TryClearWindow(bool isClearColor, bool isClearDepth) {
	if (isClearWindow_) {
		return;
	}

	ClearWindow(isClearColor, isClearDepth);
}

void DirectXWindowContext::Present(bool isTransitonAllocator) {
	if (isTransitonAllocator) {
		threadContext_->TransitionAllocator();
	}
	swapChain_->Present();

	isClearWindow_ = false;
}

void DirectXWindowContext::CheckSupportHDR(const HWND& hwnd) {

	if (swapChain_ == nullptr) {
		Logger::WarningRuntime("[DirectXWindowContext] warning | window is not create.");
		return; //!< windowが生成されていない.
	}

	RECT rect = {};
	GetWindowRect(hwnd, &rect);

	ComPtr<IDXGIOutput> output;
	std::optional<int32_t> outputArea = std::nullopt;

	// 各displayの確認
	{
		ComPtr<IDXGIOutput> current;

		for (UINT i = 0;
			SxavengerSystem::GetDxDevice()->GetAdapter()->EnumOutputs(i, &current) != DXGI_ERROR_NOT_FOUND;
			++i) {

			// displayの情報を取得
			DXGI_OUTPUT_DESC desc = {};
			auto hr = current->GetDesc(&desc);
			Exception::Assert(SUCCEEDED(hr), "DXGI_OUTPUT_DESC GetDesc() failed.");

			const Vector2i leftTop     = { desc.DesktopCoordinates.left, desc.DesktopCoordinates.top };
			const Vector2i rightBottom = { desc.DesktopCoordinates.right, desc.DesktopCoordinates.bottom };

			int32_t currentArea = ComputeIntersectionArea(
				{ rect.left, rect.top }, { rect.right, rect.bottom },
				leftTop, rightBottom
			);

			// displayの位置がウィンドウの位置と重なっているか確認
			if (currentArea > outputArea.value_or(-1)) {
				outputArea = currentArea;
				output     = current;
			}
		}
	}
	
	ComPtr<IDXGIOutput6> output6;
	auto hr = output.As(&output6);
	if (FAILED(hr)) {
		Logger::WarningRuntime("[DirectXWindowContext] warning | IDXGIOutput6 is not supported.");
		return; //!< HDRがサポートされていない
	}

	DXGI_OUTPUT_DESC1 desc = {};
	hr = output6->GetDesc1(&desc);
	if (FAILED(hr)) {
		Logger::WarningRuntime("[DirectXWindowContext] warning | IDXGIOutput6 GetDesc1() failed.");
		return; //!< HDRの情報を取得できなかった
	}

	// HDRのサポートを確認
	colorSpace_   = desc.ColorSpace;
	minLuminance_ = desc.MinLuminance;
	maxLuminance_ = desc.MaxLuminance;

}

int32_t DirectXWindowContext::ComputeIntersectionArea(
	const Vector2i& leftTopA, const Vector2i& rightBottomA,
	const Vector2i& leftTopB, const Vector2i& rightBottomB) {

	return std::max(0, std::min(rightBottomA.x, rightBottomB.x) - std::max(leftTopA.x, leftTopB.x))
		* std::max(0, std::min(rightBottomA.y, rightBottomB.y) - std::max(leftTopA.y, leftTopB.y));
	
}
