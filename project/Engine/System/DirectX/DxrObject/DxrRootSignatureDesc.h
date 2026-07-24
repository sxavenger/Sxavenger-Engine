#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxRootSignatureDesc.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 全レイトレシェーダーで共有するグローバルルートシグネチャの記述子
struct GlobalRootSignatureDesc
	: public DxObject::BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	//! @brief 指定indexにCBVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(b#)
	//! @param[in] registerSpace  レジスタスペース
	void SetVirtualCBV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 指定indexにSRVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(t#)
	//! @param[in] registerSpace  レジスタスペース
	void SetVirtualSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 指定indexにUAVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(u#)
	//! @param[in] registerSpace  レジスタスペース
	void SetVirtualUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	//! @brief 指定indexにSRVをディスクリプタハンドルとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(t#)
	//! @param[in] registerSpace  レジスタスペース
	void SetHandleSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 指定indexにUAVをディスクリプタハンドルとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(u#)
	//! @param[in] registerSpace  レジスタスペース
	void SetHandleUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* root signature *//

	//! @brief この記述からグローバルルートシグネチャを生成する
	//! @param[in] device DirectXデバイス
	//! @return 生成したルートシグネチャ
	ComPtr<ID3D12RootSignature> CreateGlobalRootSignature(ID3D12Device* device) const;


};

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エクスポート単位で個別に設定するローカルルートシグネチャの記述子
struct LocalRootSignatureDesc
	: public DxObject::BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	//! @brief 指定indexにCBVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(b#)
	void SetVirtualCBV(uint32_t index, UINT shaderRegister);
	//! @brief 指定indexにSRVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(t#)
	void SetVirtualSRV(uint32_t index, UINT shaderRegister);
	//! @brief 指定indexにUAVをGPU仮想アドレスとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(u#)
	void SetVirtualUAV(uint32_t index, UINT shaderRegister);

	//* handle *//

	//! @brief 指定indexにSRVをディスクリプタハンドルとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(t#)
	void SetHandleSRV(uint32_t index, UINT shaderRegister);
	//! @brief 指定indexにUAVをディスクリプタハンドルとして設定する
	//! @param[in] index         ルートパラメータのindex
	//! @param[in] shaderRegister シェーダーレジスタ番号(u#)
	void SetHandleUAV(uint32_t index, UINT shaderRegister);

	//* root signature *//

	//! @brief この記述からローカルルートシグネチャを生成する
	//! @param[in] device DirectXデバイス
	//! @return 生成したルートシグネチャ
	ComPtr<ID3D12RootSignature> CreateLocalRootSignature(ID3D12Device* device) const;

};

DXROBJECT_NAMESPACE_END
