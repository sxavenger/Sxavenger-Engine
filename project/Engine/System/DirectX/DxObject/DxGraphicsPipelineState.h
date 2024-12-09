#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxCompileBlobCollection.h"
#include "DxBlendState.h"
#include "DxRootSignatureDesc.h"
#include "DxBindBuffer.h"

//* lib
#include <Lib/Environment.h>
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>
#include <memory>
#include <optional>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum class PrimitiveType {
	kLine,
	kTriangle,
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

	void SetBlendMode(uint32_t renderTargetIndex, BlendMode mode);
	void SetIndependentBlendEnable(bool isIndependentEnable);

	void SetPrimitive(PrimitiveType type);

	void SetRTVFormat(DXGI_FORMAT format);
	void SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]);
	void SetDSVFormat(DXGI_FORMAT format);

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

	std::array<BlendMode, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> blendModes;
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
class GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsPipelineState() = default;
	~GraphicsPipelineState() { Term(); }

	void Term();

	//* create methods *//

	void CreateBlob(const std::filesystem::path& filename, GraphicsShaderType type);

	void CreateRootSignature(Device* device, GraphicsRootSignatureDesc& desc);

	void CreatePipeline(Device* device, const GraphicsPipelineDesc& desc);

	//* update methods *//

	void ReloadShader();

	virtual void CheckAndUpdatePipeline();

	//* setting pipeline *//

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const Vector2ui& windowSize = kMainWindowSize) const;
	void SetPipeline(CommandContext* context, const Vector2ui& windowSize = kMainWindowSize) const;

	void ReloadAndSetPipeline(ID3D12GraphicsCommandList* commandList, const Vector2ui& windowSize = kMainWindowSize);
	void ReloadAndSetPipeline(CommandContext* context, const Vector2ui& windowSize = kMainWindowSize);


	//* external methods *//

	static void SetExternal(CompileBlobCollection* collection, BlendState* blendState);

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsBlob structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicsBlob {
		std::optional<std::weak_ptr<ComPtr<IDxcBlob>>> blob;
		std::filesystem::path                          filename;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	static CompileBlobCollection* collection_;
	static BlendState* blendState_;
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
	// protected methods
	//=========================================================================================

	void CreateRootSignature();
	void CreatePipeline();

	//* option *//

	IDxcBlob* GetBlob(GraphicsShaderType type);
	D3D12_SHADER_BYTECODE GetBytecode(GraphicsShaderType type, bool isRequired = false);
	D3D12_BLEND_DESC GetBlendDesc() const;

	bool CheckShaderReloadStatus();

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

	void BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc);

	//* update methods *//

	void CheckAndUpdatePipeline() override;

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

_DXOBJECT_NAMESPACE_END