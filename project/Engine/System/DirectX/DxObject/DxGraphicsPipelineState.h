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
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>
#include <filesystem>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using BlendOption = std::variant<BlendMode, D3D12_RENDER_TARGET_BLEND_DESC>;

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum class PrimitiveType {
	PointList,
	LineList,
	LineStrip,
	TrianglList,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GraphicsPipelineDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineDesc()  = default;
	~GraphicsPipelineDesc() = default;

	//* desc setting option *//

	void SetElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot = 0);
	void ClearElement();

	void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);
	void SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_LESS);

	void SetBlendMode(uint8_t renderTargetIndex, BlendMode mode);
	void SetBlendDesc(uint8_t renderTargetIndex, const D3D12_RENDER_TARGET_BLEND_DESC& desc);
	void SetIndependentBlendEnable(bool isIndependentEnable);

	void SetPrimitive(PrimitiveType type);

	void ClearRTVFormat();
	void SetRTVFormat(DXGI_FORMAT format);
	void SetRTVFormat(uint8_t index, DXGI_FORMAT format);
	void SetRTVFormats(uint8_t size, const DXGI_FORMAT formats[]);

	void SetDSVFormat(DXGI_FORMAT format);

	void CreateDefaultDesc();

	//* getter *//

	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* descs *//

	std::vector<D3D12_INPUT_ELEMENT_DESC> elements;              //!< InputLayoutDesc
	D3D12_RASTERIZER_DESC                 rasterizerDesc   = {}; //!< RasterizerDesc
	D3D12_DEPTH_STENCIL_DESC              depthStencilDesc = {}; //!< DepthStencilDesc

	//* blends *//

	std::array<BlendOption, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> blends;
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
	void SetPipeline(CommandContext* context, const Vector2ui& windowSize = SXAVENGER_ENGINE Configuration::GetConfig().resolution) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* blob *//

	std::array<std::optional<ShaderBlob>, static_cast<uint8_t>(GraphicsShaderType::ps) + 1> blobs_;

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

	D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendDesc(const BlendOption& option) const;
	D3D12_BLEND_DESC GetBlendDesc() const;

	//* methods *//

	void CreateDirectXRootSignature(Device* device);
	void CreateDirectXRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag);

	void CreateDirectXPipeline(Device* device);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionGraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class ReflectionGraphicsPipelineState
	: public GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReflectionGraphicsPipelineState()  = default;
	~ReflectionGraphicsPipelineState() = default;

	//* reflection methods *//

	void ReflectionRootSignature(Device* device);
	void ReflectionRootSignature(Device* device, const SamplerBindDesc& desc);
	void ReflectionRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag);

	void BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc);

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
