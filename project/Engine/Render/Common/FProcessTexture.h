#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセス用のUAV/SRVテクスチャ. ミップ生成やRenderTextureとの相互コピーを提供する
class FProcessTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Argument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief プロセステクスチャ生成時の設定(解像度/フォーマット/最大ミップレベル)
	struct Argument {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 別のArgumentと互換(再利用可能)かを判定する
		//! @param[in] other 比較対象
		//! @retval true  互換あり
		//! @retval false 互換なし
		bool Compatible(const Argument& other) const;

		//! @brief 実際に生成するミップレベル数を取得する
		//! @return ミップレベル数
		uint16_t GetMiplevel() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui resolution = {};
		DXGI_FORMAT format   = DxObject::kDefaultOffscreenFormat;

		std::optional<uint16_t> maxMiplevel = std::nullopt; //!< miplevelの最大値.
		//! [std::nullopt]: 最大miplevel
		//! [value]: 指定されたmiplevelまで生成

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定設定でリソースとビューを生成する
	//! @param[in] argument 生成設定
	void Create(const Argument& argument);

	//! @brief デバッグ用のリソース名を設定する
	void SetName(const std::wstring& name) const;

	//* transition option *//
	// 各遷移は Get(バリアを返すのみ) / Set(barriers配列へ追加) / Transition(即時発行) の3形態を提供する.

	//! @brief 指定ステートへの遷移バリアを取得する (不要ならnullopt)
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionState(D3D12_RESOURCE_STATES state);
	//! @brief 指定ステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state);
	//! @brief 指定ステートへ即時遷移する
	void TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state);

	//! @brief 既定ステート(シェーダーリソース)への遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDefaultState();
	//! @brief 既定ステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief 既定ステートへ即時遷移する
	void TransitionDefaultState(const DirectXQueueContext* context);

	//! @brief UnorderedAccessステートへの遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionUnorderedAccess();
	//! @brief UnorderedAccessステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief UnorderedAccessステートへ即時遷移する
	void TransitionUnorderedAccess(const DirectXQueueContext* context);

	//! @brief UAVの書き込み完了を保証するUAVバリアを発行する
	void BarrierUnorderedAccess(const DirectXQueueContext* context);

	//* process option *//

	//! @brief ミップマップを生成する
	//! @param[in] context  DirectXのキューコンテキスト
	//! @param[in] miplevel 生成する最大ミップレベル (nulloptで全レベル)
	void GenerateMipmap(const DirectXQueueContext* context, const std::optional<uint16_t>& miplevel = std::nullopt);

	//! @brief [Render Texture] => [Process Texture] のコピー.
	void ImportFromRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture);

	//! @brief [Process Texture] => [Render Texture] のコピー.
	void ExportToRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture);

	//* getter *//

	//! @brief 内部のD3D12リソースを取得する
	ID3D12Resource* GetResource() const { return resource_.Get(); }

	//! @brief SRVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorSRV() const;
	//! @brief SRVのGPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	//! @brief 指定ミップレベルのUAV Descriptorを取得する
	//! @param[in] miplevel 対象のミップレベル
	const DxObject::Descriptor& GetDescriptorUAV(uint16_t miplevel = 0) const;
	//! @brief 指定ミップレベルのUAV GPUハンドルを取得する
	//! @param[in] miplevel 対象のミップレベル
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV(uint16_t miplevel = 0) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorSRV_;
	std::vector<DxObject::Descriptor> descriptorsUAV_;

	//* argument *//

	Argument argument_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Argument& argument);
	void CreateDescriptor(const Argument& argument);

};

SXAVENGER_ENGINE_NAMESPACE_END
