#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxDescriptor.h"

//* lib
#include <Lib/CXXAttribute.h>

//* c++
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Resource class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ID3D12Resourceと現在のリソースステートをまとめて管理するラッパークラス
class Resource {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Resource() = default;

	//* resource option *//

	//! @brief コミット済みリソース(専用ヒープ付き)を生成する
	//! @param[in] device     DirectXデバイス
	//! @param[in] prop       ヒーププロパティ
	//! @param[in] desc       リソース記述
	//! @param[in] state      初期リソースステート
	//! @param[in] clearValue 高速クリア用の値 (任意)
	void CreateCommitted(
		DxObject::Device* device,
		const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	//! @brief リソースを解放する
	void Reset();

	//! @brief デバッグ用のリソース名を設定する
	void SetName(const std::wstring& name) const;

	//! @brief リソースをCPUからアクセス可能にマップする
	//! @param[out] data  マップ先ポインタの格納先
	//! @param[in]  range マップする範囲 (任意)
	void Map(void** data, const std::optional<D3D12_RANGE>& range = std::nullopt);

	//! @brief マップを解除する
	//! @param[in] range 書き込んだ範囲 (任意)
	void Unmap(const std::optional<D3D12_RANGE>& range = std::nullopt);

	//* transition options *//

	//! @brief 指定ステートへの遷移バリアを取得する (現ステートと同じならnullopt)
	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransition(D3D12_RESOURCE_STATES state);

	//! @brief 指定ステートへの遷移バリアをbarriers配列へ追加する
	void SetTransition(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state);

	//! @brief 指定ステートへ即時遷移する
	void Transition(
		DxObject::CommandContext* context,
		D3D12_RESOURCE_STATES state
	);

	//! @brief before/afterを明示してステート遷移する (内部ステート追跡と異なる場合に使用)
	void TransitionExplicit(
		DxObject::CommandContext* context,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after
	);

	//* getter *//

	//! @brief 内部のID3D12Resourceを取得する
	ID3D12Resource* Get() const { return resource_.Get(); }

	//! @brief リソースのGPU仮想アドレスを取得する
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	//! @brief 現在追跡しているリソースステートを取得する
	D3D12_RESOURCE_STATES GetCurrentState() const { return current_; }

	//! @brief リソース記述(D3D12_RESOURCE_DESC)を取得する
	const D3D12_RESOURCE_DESC GetDesc() const;

	//* helper create methods *//

	//! @brief バッファリソースを生成するヘルパー
	//! @param[in] device DirectXデバイス
	//! @param[in] type   ヒープタイプ
	//! @param[in] size   バッファサイズ(byte)
	//! @param[in] flags  リソースフラグ
	//! @param[in] state  初期ステート
	//! @return 生成したResource
	static Resource CreateBuffer(
		DxObject::Device* device,
		D3D12_HEAP_TYPE type, size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

	//! @brief テクスチャリソースを生成するヘルパー
	//! @param[in] device     DirectXデバイス
	//! @param[in] dimension  リソース次元
	//! @param[in] size       テクスチャサイズ(幅/高さ/深さ)
	//! @param[in] miplevels  ミップレベル数
	//! @param[in] format     フォーマット
	//! @param[in] flags      リソースフラグ
	//! @param[in] state      初期ステート
	//! @param[in] clearValue 高速クリア用の値
	//! @return 生成したResource
	static Resource CreateTexture(
		DxObject::Device* device,
		D3D12_RESOURCE_DIMENSION dimension, const Vector3ui& size, UINT16 miplevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue
	);

	//* operator [copy] (delete) *//

	Resource(const Resource&)            = delete;
	Resource& operator=(const Resource&) = delete;

	//* operator [move] *//

	Resource(Resource&& other) noexcept : resource_(std::move(other.resource_)), current_(other.current_) { other.Reset(); }
	Resource& operator=(Resource&& other) noexcept { resource_ = std::move(other.resource_); current_ = other.current_; other.Reset(); return *this; }

	//* operator [assign] *//

	Resource(std::nullopt_t) noexcept { Reset(); }
	Resource& operator=(std::nullptr_t) noexcept { Reset(); return *this; }

	//* operator [equal] *//

	bool operator==(const Resource& rhs) const { return resource_.Get() == rhs.resource_.Get(); }
	bool operator!=(const Resource& rhs) const { return resource_.Get() != rhs.resource_.Get(); }

	bool operator==(std::nullptr_t) const { return resource_.Get() == nullptr; }
	bool operator!=(std::nullptr_t) const { return resource_.Get() != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	//* state tracker *//

	D3D12_RESOURCE_STATES current_ = D3D12_RESOURCE_STATE_COMMON;

};

DXOBJECT_NAMESPACE_END
