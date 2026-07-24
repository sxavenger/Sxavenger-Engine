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

//* engine
#include <Engine/System/Configuration/Configuration.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <array>
#include <filesystem>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum class PrimitiveType {
	PointList,
	LineList,
	LineStrip,
	TriangleList,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief グラフィックスPSO生成に必要な各種ステート(入力レイアウト/ラスタライザ/ブレンド/フォーマット等)を組み立てる記述子
struct GraphicsPipelineDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineDesc()  = default;
	~GraphicsPipelineDesc() = default;

	//* desc setting option *//

	//! @brief 入力レイアウトの頂点要素を1つ追加する
	void SetElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot = 0);
	//! @brief 入力レイアウトの要素をすべてクリアする
	void ClearElement();

	//! @brief ラスタライザ(カリング/塗りつぶし)を設定する
	void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);
	//! @brief 深度ステンシルテスト(有効/書き込みマスク/比較関数)を設定する
	void SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_LESS_EQUAL);

	//! @brief 指定RTのブレンドをプリセットモードで設定する
	void SetBlendMode(uint8_t renderTargetIndex, BlendMode mode);
	//! @brief 指定RTのブレンドを詳細記述で設定する
	void SetBlendDesc(uint8_t renderTargetIndex, const D3D12_RENDER_TARGET_BLEND_DESC& desc);
	//! @brief RTごとに独立したブレンドを有効化するか設定する
	void SetIndependentBlendEnable(bool isIndependentEnable);

	//! @brief プリミティブトポロジ種別を設定する
	void SetPrimitive(PrimitiveType type);

	//! @brief RTVフォーマットをすべてクリアする
	void ClearRTVFormat();
	//! @brief RTVフォーマットを1つ追加する
	void SetRTVFormat(DXGI_FORMAT format);
	//! @brief 指定indexのRTVフォーマットを設定する
	void SetRTVFormat(uint8_t index, DXGI_FORMAT format);
	//! @brief 複数のRTVフォーマットをまとめて設定する
	void SetRTVFormats(uint8_t size, const DXGI_FORMAT formats[]);

	//! @brief DSV(深度)フォーマットを設定する
	void SetDSVFormat(DXGI_FORMAT format);

	//* getter *//

	//! @brief 設定済みの入力レイアウト記述を取得する
	//! @return D3D12_INPUT_LAYOUT_DESC
	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* descs *//

	std::vector<D3D12_INPUT_ELEMENT_DESC> elements;              //!< InputLayoutDesc
	D3D12_RASTERIZER_DESC                 rasterizerDesc   = {}; //!< RasterizerDesc
	D3D12_DEPTH_STENCIL_DESC              depthStencilDesc = {}; //!< DepthStencilDesc

	//* blends *//

	std::array<D3D12_RENDER_TARGET_BLEND_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> blends;
	bool isIndependentBlendEnable = false;

	//* primitive *//

	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
	D3D_PRIMITIVE_TOPOLOGY        primitiveTopology;

	//* format *//

	std::vector<DXGI_FORMAT> rtvFormats;
	DXGI_FORMAT              dsvFormat;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief GraphicsPipelineを管理するクラス.
class GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineState()  = default;
	~GraphicsPipelineState() { Term(); }

	void Term();

	//* create methods *//

	void CreateBlob(const std::filesystem::path& filepath, GraphicsShaderType type);
	void SetBlob(const ShaderBlob& blob, GraphicsShaderType type);

	void CreateRootSignature(Device* device, GraphicsRootSignatureDesc&& desc); //!< rootSignatureDescはmove前提
	void CreateRootSignature(Device* device, GraphicsRootSignatureDesc&& desc, D3D12_ROOT_SIGNATURE_FLAGS flag);

	void CreatePipeline(Device* device, const GraphicsPipelineDesc& desc);

	//* setting pipeline *//

	void SetPipeline(CommandContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) const;
	void SetPipeline(CommandContext* context, const Vector2ui& resolution = SXAVENGER_ENGINE Configuration::GetConfig().resolution) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* blob *//

	std::array<std::optional<ShaderBlob>, static_cast<uint8_t>(GraphicsShaderType::Pixel) + 1> blobs_;

	//* rootSignature *//

	GraphicsRootSignatureDesc   rootSignatureDesc_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	//* pipeline *//

	GraphicsPipelineDesc        pipelineDesc_;
	ComPtr<ID3D12PipelineState> pipeline_;

	//* parameter *//

	bool isUseMeshShaderPipeline_ = false;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* option *//

	D3D12_SHADER_BYTECODE GetBytecode(GraphicsShaderType type, bool isRequired = false);

	D3D12_BLEND_DESC GetBlendDesc() const;

	//* methods *//

	void CreateDirectXRootSignature(Device* device);
	void CreateDirectXRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag);

	void CreateDirectXPipeline(Device* device);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionGraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シェーダーリフレクションからルートシグネチャを自動生成するグラフィックスパイプライン
class ReflectionGraphicsPipelineState
	: public GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReflectionGraphicsPipelineState()  = default;
	~ReflectionGraphicsPipelineState() = default;

	//* reflection methods *//

	//! @brief シェーダーリフレクションからルートシグネチャを構築する
	//! @param[in] device DirectXデバイス
	void ReflectionRootSignature(Device* device);
	//! @brief サンプラー記述を加えてルートシグネチャを構築する
	//! @param[in] device DirectXデバイス
	//! @param[in] desc   サンプラーバインド記述
	void ReflectionRootSignature(Device* device, const SamplerBindDesc& desc);
	//! @brief ルートシグネチャフラグを指定して構築する
	//! @param[in] device DirectXデバイス
	//! @param[in] flag   ルートシグネチャフラグ
	void ReflectionRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag);

	//! @brief 名前解決に基づき描画用バッファをバインドする
	//! @param[in] context コマンドコンテキスト
	//! @param[in] desc    バインドするバッファ記述
	void BindGraphicsBuffer(const CommandContext* context, const BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	BindBufferTable table_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void TrySetBlobToTable(GraphicsShaderType type, ShaderVisibility visibility, bool isRequired = false);

};

DXOBJECT_NAMESPACE_END
