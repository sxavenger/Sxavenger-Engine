#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxCompileBlobCollection.h"
#include "DxRootSignatureDesc.h"

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsShaderType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GraphicsShaderType {
	vs,
	gs,
	ms,
	as,
	ps
};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum class PrimitiveType {
	Line,
	Triangle,
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

	void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	void SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL);

	void SetPrimitive(PrimitiveType type);

	void SetRTVFormat(DXGI_FORMAT format);
	void SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]);
	void SetDSVFormat(DXGI_FORMAT format);

	void SetViewport(const Vector2ui& size);

	void CreateDefaultDesc();

	//* getter *//

	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;

	//=========================================================================================
	// public varaibles
	//=========================================================================================

	//* descs *//

	std::vector<D3D12_INPUT_ELEMENT_DESC> elements;              //!< InputLayoutDesc
	D3D12_RASTERIZER_DESC                 rasterizerDesc   = {}; //!< RasterizerDesc
	D3D12_DEPTH_STENCIL_DESC              depthStencilDesc = {}; //!< DepthStencilDesc

	//* blend mode *//

	//BlendMode blendMode;

	//* primitive *//

	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
	D3D_PRIMITIVE_TOPOLOGY        primitiveTopology;

	//* format *//

	std::vector<DXGI_FORMAT> rtvFormats;
	DXGI_FORMAT              dsvFormat;

	//* viewports *//

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT     rect     = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineState()  = default;
	~GraphicsPipelineState() { Term(); }

	void CreateBlob(const std::wstring& filename, GraphicsShaderType type);

	void CreateRootSignature(Device* device, const GraphicsRootSignatureDesc& desc);

	void CreatePipeline(Device* device, const GraphicsPipelineDesc& desc);

	void CheckAndUpdatePipeline();

	void Term();

	//* setter *//

	void SetPipeline(CommandContext* context) const;
	void SetPipeline(ID3D12GraphicsCommandList* commandList) const;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsBlob structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicsBlob {
		std::optional<std::weak_ptr<ComPtr<IDxcBlob>>> blob;
		std::wstring                                   filename;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	static CompileBlobCollection* collection_;
	Device* device_ = nullptr;

	//* blob *//

	std::array<GraphicsBlob, static_cast<uint32_t>(GraphicsShaderType::ps) + 1> blobs_;

	//* rootSignature *//

	ComPtr<ID3D12RootSignature> rootSignature_;
	GraphicsRootSignatureDesc   rootSignatureDesc_;

	//* pipeline *//

	ComPtr<ID3D12PipelineState> pipeline_;
	GraphicsPipelineDesc        pipelineDesc_;

	//* parameter *//

	bool isUseMeshShaderPipeline_ = false;


	//=========================================================================================
	// private methods
	//=========================================================================================

	static CompileProfile GetProfile(GraphicsShaderType type);

	void CreateRootSignature();
	void CreatePipeline();

	D3D12_SHADER_BYTECODE GetBytecode(GraphicsShaderType type, bool isRequired = false);

	bool CheckShaderHotReload() const;

};

_DXOBJECT_NAMESPACE_END