#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* c++
#include <vector>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferType enum class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シェーダーレコードへ書き込むバインド種別 (仮想アドレス/ディスクリプタハンドル)
enum class WriteBindBufferType : bool {
	VirtualAddress,
	DescriptorHandle,
};

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シェーダーレコードへ書き込むバインドリソース(アドレス/ハンドル)をindex単位で保持する記述子
struct WriteBindBufferDesc {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Container = std::vector<std::optional<DxObject::GPUBuffer>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	WriteBindBufferDesc() { Reserve(12); }

	//! @brief 指定indexにGPUバッファを設定する
	//! @param[in] index  ルートパラメータのindex
	//! @param[in] buffer 設定するGPUバッファ
	void SetBuffer(uint32_t index, const std::optional<DxObject::GPUBuffer>& buffer);
	//! @brief 指定indexにGPU仮想アドレスを設定する
	//! @param[in] index   ルートパラメータのindex
	//! @param[in] address GPU仮想アドレス
	void SetAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	//! @brief 指定indexにディスクリプタハンドルを設定する
	//! @param[in] index  ルートパラメータのindex
	//! @param[in] handle GPUディスクリプタハンドル
	void SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	//! @brief コンテナの容量を予約する
	//! @param[in] size 予約サイズ
	void Reserve(uint32_t size) { container_.reserve(size); }

	//* getter *//

	//! @brief 指定indexのGPUバッファを取得する
	//! @param[in] index ルートパラメータのindex
	//! @return GPUバッファ
	DxObject::GPUBuffer GetBuffer(uint32_t index) const;
	//! @brief 指定indexのGPU仮想アドレスを取得する
	//! @param[in] index ルートパラメータのindex
	//! @return GPU仮想アドレス
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress(uint32_t index) const;
	//! @brief 指定indexのディスクリプタハンドルを取得する
	//! @param[in] index ルートパラメータのindex
	//! @return GPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandle(uint32_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Container container_;

	void AutoResize(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// WriteBindBufferTable class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 各indexのバインド種別(アドレス/ハンドル)を定義し, descから書き込み対象を抽出するテーブル
class WriteBindBufferTable {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Table = std::vector<WriteBindBufferType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief テーブルサイズを変更する
	//! @param[in] size 要素数
	void Resize(uint32_t size) { table_.resize(size); }

	//! @brief 指定indexにバインド種別を登録する
	//! @param[in] index 対象index
	//! @param[in] type  バインド種別
	void Register(uint32_t index, WriteBindBufferType type);
	//! @brief 指定indexを仮想アドレスとして登録する
	//! @param[in] index 対象index
	void RegisterAddress(uint32_t index);
	//! @brief 指定indexをディスクリプタハンドルとして登録する
	//! @param[in] index 対象index
	void RegisterHandle(uint32_t index);

	//* getter *//

	//! @brief テーブル定義に従い, descから書き込むべきバッファ群を抽出する
	//! @param[in] desc 抽出元のバインド記述子
	//! @return 書き込み対象バッファのコンテナ
	WriteBindBufferDesc::Container GetWriteBuffers(const WriteBindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Table table_;
};



DXROBJECT_NAMESPACE_END
