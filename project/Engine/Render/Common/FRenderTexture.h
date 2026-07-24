#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color4.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief RTV/UAV/SRVを兼ねられる汎用レンダーテクスチャ. リソースとステート遷移を管理する
class FRenderTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief テクスチャに付与するビュー(用途)のフラグ
	enum class Flag : uint8_t {
		None            = 0,
		RenderTarget    = 1 << 0,
		UnorderedAccess = 1 << 1,
		ShaderResource  = 1 << 2,

		All = RenderTarget | UnorderedAccess | ShaderResource
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief レンダーテクスチャ生成時の設定(解像度/フォーマット/用途フラグ/クリア色)
	struct Option {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief フラグに対応するD3D12リソースフラグを取得する
		//! @return D3D12_RESOURCE_FLAGS
		D3D12_RESOURCE_FLAGS GetResourceFlags() const;

		//! @brief レンダーターゲット時のクリア値を取得する
		//! @return クリア値 (RenderTargetでなければnullopt)
		std::optional<D3D12_CLEAR_VALUE> GetClearValue() const;

		//! @brief 別のOptionと互換(同一設定で再利用可能)かを判定する
		//! @param[in] other 比較対象
		//! @retval true  互換あり
		//! @retval false 互換なし
		bool Compatible(const Option& other) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui   resolution = {};
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		Sxl::Flag<Flag> flag = Flag::All;
		Color4f clearColor   = {};

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderTexture() = default;
	FRenderTexture(const Vector2ui& resolution, DXGI_FORMAT format, Sxl::Flag<Flag> flag = Flag::All, Color4f clearColor = {}) { Create({ .resolution = resolution, .format = format, .flag = flag, .clearColor = clearColor }); }

	//! @brief 指定オプションでリソースとビューを生成する
	//! @param[in] option 生成設定
	void Create(const Option& option);

	//! @brief リソースとビューを破棄する
	void Reset();

	//! @brief デバッグ用のリソース名を設定する (wide文字版)
	void SetName(const std::wstring& name) const;
	//! @brief デバッグ用のリソース名を設定する
	void SetName(const std::string& name) const;

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

	//! @brief レンダーターゲットステートへの遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionRenderTarget();
	//! @brief レンダーターゲットステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionRenderTarget(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief レンダーターゲットステートへ即時遷移する
	void TransitionRenderTarget(const DirectXQueueContext* context);

	//! @brief レンダーターゲットをクリア色で塗りつぶす
	void ClearRenderTarget(const DirectXQueueContext* context);

	//! @brief UnorderedAccessステートへの遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionUnorderedAccess();
	//! @brief UnorderedAccessステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief UnorderedAccessステートへ即時遷移する
	void TransitionUnorderedAccess(const DirectXQueueContext* context);

	//! @brief UAVの書き込み完了を保証するUAVバリアを発行する
	void BarrierUnorderedAccess(const DirectXQueueContext* context);

	//* getter *//

	//! @brief 内部のD3D12リソースを取得する
	ID3D12Resource* GetResource() const { return resource_.Get(); }

	//! @brief RTVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorRTV() const;
	//! @brief RTVのCPUハンドルを取得する
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const;

	//! @brief SRVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorSRV() const;
	//! @brief SRVのGPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	//! @brief UAVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorUAV() const;
	//! @brief UAVのGPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	//! @brief テクスチャのフォーマットを取得する
	DXGI_FORMAT GetFormat() const { return option_.format; }

	//! @brief テクスチャの解像度を取得する
	const Vector2ui& GetResolution() const { return option_.resolution; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorRTV_;
	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Option option_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Option& option);
	void CreateDescriptor(const Option& option);

};

SXAVENGER_ENGINE_NAMESPACE_END
