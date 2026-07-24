#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxBlendState.h"
#include "DxShaderBlob.h"
#include "DxRootSignatureDesc.h"
#include "DxBindBuffer.h"

//* lib
#include <Lib/Math/Vector3.h>

//* c++
#include <filesystem>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DirectXのComputePipelineを作成, 設定するクラス
class ComputePipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ComputePipelineState() = default;
	~ComputePipelineState() { Term(); }

	void Term();

	//* create methods *//

	void SetBlob(const ShaderBlob& blob);
	void CreateBlob(const std::filesystem::path& filepath);

	void CreatePipeline(Device* device, ComputeRootSignatureDesc&& desc); //!< rootSignatureDescはmove前提

	//* setting pipeline *//

	void SetPipeline(CommandContext* context) const;

	void Dispatch(CommandContext* context, const Vector3ui& threadGroupCount) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	ShaderBlob blob_;

	//* rootSignature *//

	ComputeRootSignatureDesc    rootSignatureDesc_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	//* pipeline *//

	ComPtr<ID3D12PipelineState> pipeline_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* option *//

	D3D12_SHADER_BYTECODE GetBytecode();

	//* methods *//

	void CreateDirectXRootSignature(Device* device);

	void CreateDirectXPipeline(Device* device);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シェーダーリフレクションからルートシグネチャを自動生成するコンピュートパイプライン
class ReflectionComputePipelineState
	: public ComputePipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReflectionComputePipelineState()  = default;
	~ReflectionComputePipelineState() = default;

	//* reflection methods *//

	//! @brief シェーダーリフレクションからパイプライン(ルートシグネチャ)を構築する
	//! @param[in] device DirectXデバイス
	void ReflectionPipeline(Device* device);

	//! @brief サンプラー記述を加えてパイプラインを構築する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   サンプラーバインド記述
	void ReflectionPipeline(Device* device, const SamplerBindDesc& desc);

	//! @brief 名前解決に基づきコンピュート用バッファをバインドする
	//! @param[in] context コマンドコンテキスト
	//! @param[in] desc    バインドするバッファ記述
	void BindComputeBuffer(const CommandContext* context, const BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	BindBufferTable table_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void SetBlobToTable();
};


DXOBJECT_NAMESPACE_END
