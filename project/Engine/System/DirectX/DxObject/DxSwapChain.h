#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"
#include "DxDescriptorHeaps.h"
#include "DxCommandContext.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief スワップチェーンとバックバッファ(RTV)を管理し, 画面へのpresentを担うクラス
class SwapChain {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ColorSpace enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 出力の色空間(SDR/HDR)種別
	enum class ColorSpace : uint32_t {
		Rec_709,
		Rec_2020_1000nit,
		Rec_2020_2000nit,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1つのバックバッファ(リソースとRTV)を保持する構造体
	struct Buffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief バッファを解放する
		void Reset();

		//=========================================================================================
		// public variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorRTV;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SwapChain()  = default;
	~SwapChain() { Term(); }

	//! @brief スワップチェーンとバックバッファを生成する
	//! @param[in] device          DirectXデバイス
	//! @param[in] descriptorHeaps ディスクリプタヒープ
	//! @param[in] command         コマンドコンテキスト
	//! @param[in] format          バッファフォーマット
	//! @param[in] size            バッファサイズ
	//! @param[in] hwnd            出力先ウィンドウハンドル
	void Init(
		Device* device, DescriptorHeaps* descriptorHeaps, CommandContext* command,
		DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd
	);

	//! @brief スワップチェーンのバッファサイズを変更する
	//! @param[in] device          DirectXデバイス
	//! @param[in] descriptorHeaps ディスクリプタヒープ
	//! @param[in] format          バッファフォーマット
	//! @param[in] size            新しいバッファサイズ
	void Resize(
		Device* device, DescriptorHeaps* descriptorHeaps,
		DXGI_FORMAT format, const Vector2ui& size
	);

	//! @brief スワップチェーンとバッファを破棄する
	void Term();

	//! @brief バックバッファを画面へ表示(present)する
	void Present();

	//! @brief 現在のバックバッファのindexを取得する
	//! @return バックバッファindex
	UINT GetCurrentBackBufferIndex() const;

	//! @brief 出力ディスプレイの情報から色空間(HDR等)を設定する
	//! @param[in] desc ディスプレイ出力の記述
	void SetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	//* getter *//

	//! @brief 現在のバックバッファのステート遷移バリアを取得する
	//! @param[in] stateBefore 遷移前ステート
	//! @param[in] stateAfter  遷移後ステート
	//! @return リソースバリア
	D3D12_RESOURCE_BARRIER GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const;

	//! @brief 現在のバックバッファのRTV CPUハンドルを取得する
	//! @return RTVのCPUディスクリプタハンドル
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetBackBufferCPUHandle() const;

	//! @brief 現在の色空間を取得する
	//! @return 色空間
	const ColorSpace GetColorSpace() const { return colorSpace_; }

	//! @brief バックバッファ数を取得する
	//! @return バッファ数
	static const UINT GetBufferCount() { return kBufferCount; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDXGISwapChain4> swapChain_;

	//* buffers *//

	static const UINT kBufferCount = 2;
	std::array<Buffer, kBufferCount> buffers_;

	//* parameter *//

	ColorSpace colorSpace_ = ColorSpace::Rec_709;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static std::optional<ColorSpace> GetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	static UINT16 GetChromaticity(double v);

	//* helper methods *//

	void CreateSwapChain(Device* device, CommandContext* command, DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd);
	void ResizeSwapChain(DXGI_FORMAT format, const Vector2ui& size);

	void CreateBuffer(Device* device, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format, bool isSRGB);
};

DXOBJECT_NAMESPACE_END
