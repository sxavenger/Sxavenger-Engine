#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"
#include "DxrExportGroup.h"
#include "DxrBindBuffer.h"
#include "DxrAccelerationStructure.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxCommandContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* directx12
#include <d3dx12.h>

//* c++
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief レイトレStateObject生成に必要なエクスポート群とパイプライン設定を記述する構造体
struct StateObjectDesc {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief エクスポート種別ごとに要素を持つ配列型
	template <typename T>
	using ExportArray = std::array<T, kExportTypeCount>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StateObjectDesc()  = default;
	~StateObjectDesc() = default;

	//* blob option *//

	//! @brief シェーダーのエクスポートグループを登録する
	//! @param[in] expt 登録するエクスポートグループ
	void AddExport(const DxrObject::ExportGroup* expt);

	//* parameter option *//

	//! @brief レイのペイロードサイズを設定する
	//! @param[in] stride ペイロードのバイトサイズ
	void SetPayloadStride(size_t stride);

	//! @brief 交差アトリビュートのサイズを設定する
	//! @param[in] stride アトリビュートのバイトサイズ
	void SetAttributeStride(size_t stride);

	//! @brief レイの最大再帰深度を設定する
	//! @param[in] depth 最大再帰深度
	void SetMaxRecursionDepth(uint8_t depth);

	//* getter *//

	//! @brief 指定種別の全エクスポートを取得する
	//! @param[in] type エクスポート種別
	//! @return 名前をキーとしたエクスポートのマップ
	const std::unordered_map<std::string, const DxrObject::ExportGroup*>& GetExports(ExportType type) const {return exports_[static_cast<size_t>(type)];}

	//! @brief 指定種別・名前のエクスポートを取得する
	//! @param[in] type エクスポート種別
	//! @param[in] name エクスポート名
	//! @return エクスポートグループへのポインタ
	const DxrObject::ExportGroup* GetExport(ExportType type, const std::string& name) const;

	//! @brief 指定種別のエクスポート数を取得する
	//! @param[in] type エクスポート種別
	//! @return エクスポート数
	size_t GetExportCount(ExportType type) const { return exports_[static_cast<size_t>(type)].size(); }

	//! @brief 指定種別の最大ローカルルート引数strideを取得する
	//! @param[in] type エクスポート種別
	//! @return stride(バイト)
	size_t GetStride(ExportType type) const { return strides_[static_cast<size_t>(type)]; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* parameters *//

	size_t payloadStride      = NULL;
	size_t attributeStride    = NULL;
	uint8_t maxRecursionDepth = NULL;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export parameters *//

	ExportArray<std::unordered_map<std::string, const DxrObject::ExportGroup*>> exports_ = {}; //!< exportのコンテナ
	ExportArray<size_t>                                                         strides_ = {}; //!< exportごとの最大stride

};

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectContext class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief レイトレのStateObject/RootSignature/シェーダーテーブルを構築し, DispatchRaysを実行するコンテキスト
class StateObjectContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StateObjectContext()  = default;
	~StateObjectContext() = default;

	//* root signature option *//

	//! @brief グローバルルートシグネチャを生成する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   ルートシグネチャ記述
	void CreateRootSignature(DxObject::Device* device, const GlobalRootSignatureDesc& desc);

	//* state object option *//

	//! @brief 記述からレイトレStateObjectを生成する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   StateObject記述
	void CreateStateObject(DxObject::Device* device, const StateObjectDesc& desc);
	//! @brief 記述(ムーブ)からレイトレStateObjectを生成する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   StateObject記述(ムーブ)
	void CreateStateObject(DxObject::Device* device, StateObjectDesc&& desc);

	//* shader table option *//

	//! @brief TLASのインスタンスに合わせてシェーダーテーブルを構築/更新する
	//! @param[in] device        DirectXデバイス
	//! @param[in] toplevelAS    対象のTLAS
	//! @param[in] raygeneration RayGenerationのバインド記述 (任意)
	//! @param[in] miss          Missのバインド記述 (任意)
	void UpdateShaderTable(
		DxObject::Device* device,
		const DxrObject::TopLevelAS* toplevelAS,
		const DxrObject::WriteBindBufferDesc* raygeneration = nullptr, const DxrObject::WriteBindBufferDesc* miss = nullptr //!< HACK: 複数設定できるようにする
	);

	//* render option *//

	//! @brief StateObjectとルートシグネチャをコマンドリストへ設定する
	//! @param[in] context コマンドコンテキスト
	void SetStateObject(DxObject::CommandContext* context) const;

	//! @brief レイをディスパッチする (2D)
	//! @param[in] context    コマンドコンテキスト
	//! @param[in] resolution ディスパッチする解像度
	void DispatchRays(DxObject::CommandContext* context, const Vector2ui& resolution) const;
	//! @brief レイをディスパッチする (3D)
	//! @param[in] context    コマンドコンテキスト
	//! @param[in] resolution ディスパッチする解像度(3D)
	void DispatchRays(DxObject::CommandContext* context, const Vector3ui& resolution) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* root signature *//

	ComPtr<ID3D12RootSignature> rootSignature_; //!< global root signature

	//* state object *//

	ComPtr<ID3D12StateObject>           stateObject_;
	ComPtr<ID3D12StateObjectProperties> properties_;

	//* shader table *//

	ComPtr<ID3D12Resource> shaderTable_;
	UINT                   shaderTableStride_ = NULL;

	// fixme: shader tableとstate objectを分けるべきか？

	//* desc *//

	StateObjectDesc desc_;
	D3D12_DISPATCH_RAYS_DESC dispatchDesc_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* subobject sub methods *//

	void BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindGlobalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindExportLocalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc);

	//* shader table sub methods *//

	static UINT WriteIdentifier(uint8_t* dst, const void* id);
	static UINT WriteGPUVirtualAddress(uint8_t* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	static UINT WriteGPUHandle(uint8_t* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	uint8_t* WriteExport(uint8_t* dst, UINT size, const ExportGroup* expt, const WriteBindBufferDesc* desc = nullptr);
	uint8_t* WriteExport(uint8_t* dst, UINT size, ExportType type, const std::string& name, const WriteBindBufferDesc* desc = nullptr);

};

DXROBJECT_NAMESPACE_END
