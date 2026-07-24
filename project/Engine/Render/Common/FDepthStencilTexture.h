#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthStencilTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DSV/SRVを兼ねる深度ステンシルテクスチャ. リソースとステート遷移を管理する
class FDepthStencilTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 深度ステンシル生成時の設定(解像度/フォーマット/クリア値)
	struct Option {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 別のOptionと互換(再利用可能)かを判定する
		//! @param[in] other 比較対象
		//! @retval true  互換あり
		//! @retval false 互換なし
		bool Compatible(const Option& other) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui resolution = {};

		DXGI_FORMAT format = DxObject::kDefaultDepthFormat;

		float clearDepth     = 1.0f;
		uint8_t clearStencil = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FDepthStencilTexture() = default;
	FDepthStencilTexture(const Vector2ui& resolution) { Create({ .resolution = resolution }); }

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

	//! @brief 深度書き込みステートへの遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDepthWrite();
	//! @brief 深度書き込みステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionDepthWrite(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief 深度書き込みステートへ即時遷移する
	void TransitionDepthWrite(const DirectXQueueContext* context);

	//! @brief 深度ステンシルをクリアする
	void ClearDepthStencil(const DirectXQueueContext* context);

	//! @brief 深度読み込みステートへの遷移バリアを取得する
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDepthRead();
	//! @brief 深度読み込みステートへの遷移バリアをbarriers配列へ追加する
	void SetTransitionDepthRead(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	//! @brief 深度読み込みステートへ即時遷移する
	void TransitionDepthRead(const DirectXQueueContext* context);

	//* getter *//

	//! @brief 内部のD3D12リソースを取得する
	ID3D12Resource* GetResource() const { return resource_.Get(); }

	//! @brief DSVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorDSV() const;
	//! @brief DSVのCPUハンドルを取得する
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const;

	//! @brief SRVのDescriptorを取得する
	const DxObject::Descriptor& GetDescriptorSRV() const;
	//! @brief SRVのGPUハンドルを取得する
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorDSV_;
	DxObject::Descriptor descriptorSRV_;

	//* option *//

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
