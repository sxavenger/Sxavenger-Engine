#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* c++
#include <vector>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ルートパラメータとスタティックサンプラーを組み立て, ルートシグネチャを生成する記述子の基底
struct BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseRootSignatureDesc()  { Reserve(); }
	~BaseRootSignatureDesc() = default;

	//* container option *//

	//! @brief パラメータ/サンプラーのコンテナ容量を予約する
	void Reserve(uint32_t paramsCapacity = kDefaultParamCapacity_, uint32_t samplersCapacity = kDefaultSamplerCapacity_);

	//! @brief 登録済みのパラメータ/サンプラーをクリアする
	void Clear();

	//! @brief 記述子を初期状態へリセットする
	void Reset();

	//* container setter *//

	//! @brief 指定indexにルート記述子(仮想アドレス)を設定する
	void SetVirtual(uint32_t index, ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 指定indexにディスクリプタテーブル(ハンドル)を設定する
	void SetHandle(uint32_t index, ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 指定indexにルート定数(32bit定数)を設定する
	void Set32bitConstants(uint32_t index, ShaderVisibility stage, UINT num32bit, UINT shaderRegister, UINT registerSpace = 0);

	//! @brief スタティックサンプラーを詳細記述で追加する
	void SetSamplerDesc(const D3D12_STATIC_SAMPLER_DESC& desc);
	//! @brief フィルタ/アドレッシングを指定してスタティックサンプラーを追加する
	void SetSamplerFilter(SamplerFilter filter, SamplerMode mode, ShaderVisibility stage, uint32_t anisotropic, UINT shaderRegister, UINT registerSpace = 0);

	//! @brief リニア補間サンプラーを追加する
	void SetSamplerLinear(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief ポイント(最近傍)サンプラーを追加する
	void SetSamplerPoint(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	//! @brief 異方性フィルタサンプラーを追加する
	void SetSamplerAnisotropic(SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic, UINT registerSpace = 0);

	//! @brief この記述からルートシグネチャを生成する
	//! @param[in] device DirectXデバイス
	//! @param[in] flags  ルートシグネチャフラグ
	//! @return 生成したルートシグネチャ
	virtual ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device, D3D12_ROOT_SIGNATURE_FLAGS flags) const;

	//* operator *//

	BaseRootSignatureDesc& operator=(const BaseRootSignatureDesc&) = delete;
	BaseRootSignatureDesc(const BaseRootSignatureDesc&)            = delete;

	BaseRootSignatureDesc& operator=(BaseRootSignatureDesc&&) = default;
	BaseRootSignatureDesc(BaseRootSignatureDesc&&)            = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<D3D12_ROOT_PARAMETER1>     params;
	std::vector<D3D12_DESCRIPTOR_RANGE1>   ranges_;

	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

	static const uint32_t kDefaultParamCapacity_   = 32;
	static const uint32_t kDefaultSamplerCapacity_ = 32;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void AutoResizeParam(uint32_t index);
	void AutoResizeSampler(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief グラフィックスパイプライン用のルートシグネチャ記述子 (ステージ可視性を指定可能)
struct GraphicsRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	void SetVirtualCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	void SetHandleSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandleUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace = 0);

	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(
		ID3D12Device* device,
		D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	) const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ComputeRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief コンピュートパイプライン用のルートシグネチャ記述子
struct ComputeRootSignatureDesc
	: public BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	void SetVirtualCBV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	void SetHandleSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandleUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	ComPtr<ID3D12RootSignature> CreateComputeRootSignature(ID3D12Device* device) const;


};

DXOBJECT_NAMESPACE_END
