#include "DxGraphicsPipelineState.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DirectX12
#include <d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineDesc::SetElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot) {
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = semanticName;
	element.SemanticIndex     = semanticIndex;
	element.Format            = format;
	element.InputSlot         = inputSlot;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	elements.emplace_back(element);
}

void GraphicsPipelineDesc::ClearElement() {
	elements.clear();
}

void GraphicsPipelineDesc::SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = fillMode;
}

void GraphicsPipelineDesc::SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask, D3D12_COMPARISON_FUNC func) {
	depthStencilDesc.DepthEnable    = depthEnable;
	depthStencilDesc.DepthWriteMask = writeMask;
	depthStencilDesc.DepthFunc      = func;
}

void GraphicsPipelineDesc::SetBlendMode(uint8_t renderTargetIndex, BlendMode mode) {
	blends[renderTargetIndex] = mode;
}

void GraphicsPipelineDesc::SetBlendDesc(uint8_t renderTargetIndex, const D3D12_RENDER_TARGET_BLEND_DESC& desc) {
	blends[renderTargetIndex] = desc;
}

void GraphicsPipelineDesc::SetIndependentBlendEnable(bool isIndependentEnable) {
	isIndependentBlendEnable = isIndependentEnable;
}

void GraphicsPipelineDesc::SetPrimitive(PrimitiveType type) {
	switch (type) {
		case PrimitiveType::PointList:
			primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;

		case PrimitiveType::LineList:
			primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			break;

		case PrimitiveType::LineStrip:
			primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;

		case PrimitiveType::TrianglList:
			primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
	}
}

void GraphicsPipelineDesc::SetRTVFormat(DXGI_FORMAT format) {
	rtvFormats.emplace_back(format);
	Exception::Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "RTV Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineDesc::SetRTVFormat(uint8_t index, DXGI_FORMAT format) {
	rtvFormats[index] = format;
	Exception::Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "RTV Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineDesc::SetRTVFormats(uint8_t size, const DXGI_FORMAT formats[]) {
	rtvFormats.insert(rtvFormats.end(), formats, formats + size);
	Exception::Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "RTV Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineDesc::SetDSVFormat(DXGI_FORMAT format) {
	dsvFormat = format;
}

void GraphicsPipelineDesc::CreateDefaultDesc() {
	ClearElement();
	SetElement("POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	SetElement("TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT);
	SetElement("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT);
	SetElement("TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT);
	SetElement("BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	SetDepthStencil(true);

	SetBlendMode(0, BlendMode::kBlendModeNormal);
	SetIndependentBlendEnable(false);

	SetPrimitive(PrimitiveType::TrianglList);

	SetRTVFormat(kDefaultOffscreenFormat);
	SetDSVFormat(kDefaultDepthFormat);
}

D3D12_INPUT_LAYOUT_DESC GraphicsPipelineDesc::GetInputLayout() const {
	D3D12_INPUT_LAYOUT_DESC result = {};
	result.pInputElementDescs = elements.data();
	result.NumElements        = static_cast<UINT>(elements.size());
	return result;
}

//=========================================================================================
// static variables
//=========================================================================================

BlendState* GraphicsPipelineState::blendState_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineState::Term() {
}

void GraphicsPipelineState::CreateBlob(const std::filesystem::path& filepath, GraphicsShaderType type) {
	std::unique_ptr<ShaderBlob> blob = std::make_unique<ShaderBlob>();
	blob->Create(filepath, ToProfile(type));

	SetBlob(*blob.get(), type);
}

void GraphicsPipelineState::SetBlob(const ShaderBlob& blob, GraphicsShaderType type) {
	if (type == GraphicsShaderType::ms) {
		isUseMeshShaderPipeline_ = true; //!< mesh shader pipelineを使用する場合
	}

	blobs_[static_cast<uint8_t>(type)] = blob;
}

void GraphicsPipelineState::CreateRootSignature(Device* device, GraphicsRootSignatureDesc&& desc) {
	rootSignatureDesc_ = std::move(desc);
	
	CreateDirectXRootSignature(device);
}

void GraphicsPipelineState::CreateRootSignature(Device* device, GraphicsRootSignatureDesc&& desc, D3D12_ROOT_SIGNATURE_FLAGS flag) {
	rootSignatureDesc_ = std::move(desc);

	CreateDirectXRootSignature(device, flag);
}

void GraphicsPipelineState::CreatePipeline(Device* device, const GraphicsPipelineDesc& desc) {
	pipelineDesc_ = desc;

	CreateDirectXPipeline(device);
}

void GraphicsPipelineState::SetPipeline(CommandContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) const {
	auto commandList = context->GetCommandList();

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipeline_.Get());

	if (!isUseMeshShaderPipeline_) {
		//!< vertex pipeline のときIAPrimitiveが必要になるので
		commandList->IASetPrimitiveTopology(pipelineDesc_.primitiveTopology);
	}
}

void GraphicsPipelineState::SetPipeline(CommandContext* context, const Vector2ui& windowSize) const {

	// viewportの設定
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width    = static_cast<float>(windowSize.x);
	viewport.Height   = static_cast<float>(windowSize.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形の設定
	D3D12_RECT rect = {};
	rect.left   = 0;
	rect.right  = windowSize.x;
	rect.top    = 0;
	rect.bottom = windowSize.y;

	SetPipeline(context, viewport, rect);
}

void GraphicsPipelineState::SetExternal(BlendState* blendState) {
	blendState_ = blendState;
}

D3D12_SHADER_BYTECODE GraphicsPipelineState::GetBytecode(GraphicsShaderType type, bool isRequired) {
	if (!blobs_[static_cast<uint8_t>(type)].has_value()) {
		Exception::Assert(!isRequired, "required blob is not set."); //!< 絶対的に使用されるblobが設定されていない.
		return {};
	}

	return blobs_[static_cast<uint8_t>(type)].value().GetBytecode();
}

D3D12_RENDER_TARGET_BLEND_DESC GraphicsPipelineState::GetRenderTargetBlendDesc(const BlendOption& option) const {
	if (std::holds_alternative<BlendMode>(option)) {
		BlendMode blendMode = std::get<BlendMode>(option);
		return blendState_->GetDesc(blendMode);

	} else if (std::holds_alternative<D3D12_RENDER_TARGET_BLEND_DESC>(option)) {
		return std::get<D3D12_RENDER_TARGET_BLEND_DESC>(option);
	}

	Exception::Assert(false, "is not define option.");
	return {};
}

D3D12_BLEND_DESC GraphicsPipelineState::GetBlendDesc() const {
	D3D12_BLEND_DESC desc = {};
	desc.IndependentBlendEnable = pipelineDesc_.isIndependentBlendEnable;
	desc.RenderTarget[0]        = GetRenderTargetBlendDesc(pipelineDesc_.blends[0]);

	if (desc.IndependentBlendEnable) {
		for (uint8_t i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			desc.RenderTarget[i] = GetRenderTargetBlendDesc(pipelineDesc_.blends[i]);
		}
	}

	return desc;
}

void GraphicsPipelineState::CreateDirectXRootSignature(Device* device) {
	rootSignature_ = rootSignatureDesc_.CreateGraphicsRootSignature(device->GetDevice());
}

void GraphicsPipelineState::CreateDirectXRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag) {
	rootSignature_ = rootSignatureDesc_.CreateGraphicsRootSignature(device->GetDevice(), flag);
}

void GraphicsPipelineState::CreateDirectXPipeline(Device* device) {
	if (isUseMeshShaderPipeline_) {

		//!< mesh piplineの設定
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature     = rootSignature_.Get();
		desc.RasterizerState    = pipelineDesc_.rasterizerDesc;
		desc.DepthStencilState  = pipelineDesc_.depthStencilDesc;
		desc.DSVFormat          = pipelineDesc_.dsvFormat;
		desc.SampleMask         = UINT_MAX;
		desc.SampleDesc.Count   = 1;
		desc.Flags              = D3D12_PIPELINE_STATE_FLAG_NONE;

		// blendModeの設定
		desc.BlendState = GetBlendDesc();
		
		// RTVFormatの設定
		desc.NumRenderTargets = static_cast<UINT>(pipelineDesc_.rtvFormats.size());
		std::copy(pipelineDesc_.rtvFormats.begin(), pipelineDesc_.rtvFormats.end(), desc.RTVFormats);

		// blobの設定
		desc.AS = GetBytecode(GraphicsShaderType::as);
		desc.MS = GetBytecode(GraphicsShaderType::ms, true);
		desc.PS = GetBytecode(GraphicsShaderType::ps, true);

		// pipelineの生成
		CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(desc);

		D3D12_PIPELINE_STATE_STREAM_DESC descStream = {};
		descStream.pPipelineStateSubobjectStream = &psoStream;
		descStream.SizeInBytes                   = sizeof(psoStream);

		auto hr = device->GetDevice()->CreatePipelineState(
			&descStream,
			IID_PPV_ARGS(&pipeline_)
		);

		Exception::Assert(SUCCEEDED(hr));

	} else {

		//!< vertex pipeline設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature        = rootSignature_.Get();
		desc.InputLayout           = pipelineDesc_.GetInputLayout();
		desc.RasterizerState       = pipelineDesc_.rasterizerDesc;
		desc.SampleDesc.Count      = 1;
		desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
		desc.DepthStencilState     = pipelineDesc_.depthStencilDesc;
		desc.DSVFormat             = pipelineDesc_.dsvFormat;
		desc.PrimitiveTopologyType = pipelineDesc_.primitiveTopologyType;

		// blendModeの設定
		desc.BlendState = GetBlendDesc();

		// RTVformatの設定
		desc.NumRenderTargets = static_cast<UINT>(pipelineDesc_.rtvFormats.size());
		std::copy(pipelineDesc_.rtvFormats.begin(), pipelineDesc_.rtvFormats.end(), desc.RTVFormats);

		desc.VS = GetBytecode(GraphicsShaderType::vs, true);
		desc.GS = GetBytecode(GraphicsShaderType::gs);
		desc.PS = GetBytecode(GraphicsShaderType::ps, true);

		// pipelineの生成
		auto hr = device->GetDevice()->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(&pipeline_)
		);
		Exception::Assert(SUCCEEDED(hr), "vertex shader pipeline create failed.");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionGraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ReflectionGraphicsPipelineState::ReflectionRootSignature(Device* device) {

	table_.Reset();

	if (isUseMeshShaderPipeline_) {
		TrySetBlobToTable(GraphicsShaderType::as, ShaderVisibility::VISIBILITY_AMPLIFICATION);
		TrySetBlobToTable(GraphicsShaderType::ms, ShaderVisibility::VISIBILITY_MESH, true);

	} else {
		TrySetBlobToTable(GraphicsShaderType::vs, ShaderVisibility::VISIBILITY_VERTEX, true);
		TrySetBlobToTable(GraphicsShaderType::gs, ShaderVisibility::VISIBILITY_GEOMETRY);
	}

	TrySetBlobToTable(GraphicsShaderType::ps, ShaderVisibility::VISIBILITY_PIXEL, true);

	rootSignatureDesc_ = table_.CreateGraphicsRootSignatureDesc();
	CreateDirectXRootSignature(device);
}

void ReflectionGraphicsPipelineState::ReflectionRootSignature(Device* device, const SamplerBindDesc& desc) {

	table_.Reset();

	if (isUseMeshShaderPipeline_) {
		TrySetBlobToTable(GraphicsShaderType::as, ShaderVisibility::VISIBILITY_AMPLIFICATION);
		TrySetBlobToTable(GraphicsShaderType::ms, ShaderVisibility::VISIBILITY_MESH, true);

	} else {
		TrySetBlobToTable(GraphicsShaderType::vs, ShaderVisibility::VISIBILITY_VERTEX, true);
		TrySetBlobToTable(GraphicsShaderType::gs, ShaderVisibility::VISIBILITY_GEOMETRY);
	}

	TrySetBlobToTable(GraphicsShaderType::ps, ShaderVisibility::VISIBILITY_PIXEL, true);


	rootSignatureDesc_ = table_.CreateGraphicsRootSignatureDesc(desc);
	CreateDirectXRootSignature(device);
}

void ReflectionGraphicsPipelineState::ReflectionRootSignature(Device* device, D3D12_ROOT_SIGNATURE_FLAGS flag) {
	table_.Reset();

	if (isUseMeshShaderPipeline_) {
		TrySetBlobToTable(GraphicsShaderType::as, ShaderVisibility::VISIBILITY_AMPLIFICATION);
		TrySetBlobToTable(GraphicsShaderType::ms, ShaderVisibility::VISIBILITY_MESH, true);

	} else {
		TrySetBlobToTable(GraphicsShaderType::vs, ShaderVisibility::VISIBILITY_VERTEX, true);
		TrySetBlobToTable(GraphicsShaderType::gs, ShaderVisibility::VISIBILITY_GEOMETRY);
	}

	TrySetBlobToTable(GraphicsShaderType::ps, ShaderVisibility::VISIBILITY_PIXEL, true);


	rootSignatureDesc_ = table_.CreateGraphicsRootSignatureDesc();
	CreateDirectXRootSignature(device, flag);
}

void ReflectionGraphicsPipelineState::BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc) {
	table_.BindGraphicsBuffer(context, desc);
}

void ReflectionGraphicsPipelineState::TrySetBlobToTable(GraphicsShaderType type, ShaderVisibility visibility, bool isRequired) {
	if (!blobs_[static_cast<uint8_t>(type)].has_value()) {
		Exception::Assert(!isRequired, "required blob is not set."); //!< 絶対的に使用されるblobが設定されていない.
		return; //!< blobが登録されていないのでreturn
	}

	ComPtr<ID3D12ShaderReflection> reflection = blobs_[static_cast<uint8_t>(type)].value().GetReflection();
	table_.CreateTable(reflection.Get(), visibility);
}
