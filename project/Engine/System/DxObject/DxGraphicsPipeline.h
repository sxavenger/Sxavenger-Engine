#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

//* DxObject
#include "DxBlendState.h"
#include "DxShaderBlob.h"
#include "DxRootSignatureDesc.h"

//* Lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Devices;

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType {
	PRIMITIVE_LINE,
	PRIMITIVE_TRIANGLE,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GraphicsPipelineDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineDesc() { Init(); }

	~GraphicsPipelineDesc() { Clear(); }

	void Init();

	void Clear();

	void CreateDefaultDesc();

	/* setters */

	void SetElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot = 0);

	void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	void SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL);

	void SetBlendMode(BlendMode blendMode);

	void SetPrimitive(PrimitiveType type);

	void SetRTVFormat(DXGI_FORMAT format);
	void SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]);

	/* getter */

	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;


	//=========================================================================================
	// public variables
	//=========================================================================================

	/* descs */

	std::vector<D3D12_INPUT_ELEMENT_DESC> elements;         //!< InputLayoutDesc
	D3D12_RASTERIZER_DESC                 rasterizerDesc;   //!< RasterizerDesc
	D3D12_DEPTH_STENCIL_DESC              depthStencilDesc; //!< DepthStencilDesc

	/* param */

	BlendMode blendMode;

	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
	D3D_PRIMITIVE_TOPOLOGY        primitiveTopology;

	std::vector<DXGI_FORMAT> rtvFormats;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipeline() { CreateViewports(); }
	~GraphicsPipeline() = default;

	//* Create Pipelines *//

	void CreateViewports(const Vector2ui& clientSize = kWindowSize);

	void CreateRootSignature(Devices* devices, const BaseRootSignatureDesc& graphicsDesc);

	void CreatePipeline(Devices* devices, GraphicsBlob* graphicsBlob, BlendMode blendMode); //!< use default desc
	void CreatePipeline(Devices* devices, GraphicsBlob* graphicsBlob, const GraphicsPipelineDesc& descs);

	//* set *//

	void SetPipeline(ID3D12GraphicsCommandList* commandList) const;

	static void SetBlendState(BlendState* blendState) { blendState_ = blendState; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	static BlendState* blendState_;

	//* ID3D12 *//

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipeline_;

	//* parameter *//

	bool                   isUseMeshPipeline_;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology_;

	//* viewports *//

	D3D12_VIEWPORT viewport_;
	D3D12_RECT     scissorRect_;

};

_DXOBJECT_NAMESPACE_END