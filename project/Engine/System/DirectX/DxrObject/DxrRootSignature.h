#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignatureDesc.h"
#include "DxrBindBuffer.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エクスポート単位のローカルルートシグネチャと, シェーダーレコードへの書き込みテーブルを保持するクラス
class LocalRootSignature {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LocalRootSignature()  = default;
	~LocalRootSignature() = default;

	//! @brief 記述からローカルルートシグネチャを生成する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   ローカルルートシグネチャ記述
	void CreateRootSignature(DxObject::Device* device, const LocalRootSignatureDesc& desc);

	//* getter *//

	//! @brief 生成したルートシグネチャを取得する
	//! @return ID3D12RootSignatureへのポインタ
	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	//! @brief シェーダーレコードに書き込むローカル引数のstrideを取得する
	//! @return stride(バイト)
	size_t GetBufferStride() const { return stride_; }

	//! @brief シェーダーレコードへの書き込みテーブルを取得する
	//! @return 書き込みテーブルへの参照
	const WriteBindBufferTable& GetTable() const { return table_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* root signature *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//* size *//

	size_t stride_ = NULL;

	//* table *//

	WriteBindBufferTable table_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateTable(const LocalRootSignatureDesc& desc);

};

DXROBJECT_NAMESPACE_END
