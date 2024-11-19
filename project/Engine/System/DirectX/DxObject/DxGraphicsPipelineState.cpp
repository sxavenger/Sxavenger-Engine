#include "DxGraphicsPipelineState.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DirectX12
#include <d3dx12.h>

//* lib
#include <Lib/Environment.h>

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

void GraphicsPipelineDesc::SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = fillMode;
}

void GraphicsPipelineDesc::SetDepthStencil(bool depthEnable, D3D12_DEPTH_WRITE_MASK writeMask) {
	depthStencilDesc.DepthEnable    = depthEnable;
	depthStencilDesc.DepthWriteMask = writeMask;
	depthStencilDesc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void GraphicsPipelineDesc::SetBlendMode(uint32_t renderTargetIndex, BlendMode mode) {
	blendModes[renderTargetIndex] = mode;
}

void GraphicsPipelineDesc::SetIndependentBlendEnable(bool isIndependentEnable) {
	isIndependentBlendEnable = isIndependentEnable;
}

void GraphicsPipelineDesc::SetPrimitive(PrimitiveType type) {
	if (type == PrimitiveType::Line) { //!< 線分
		primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return;

	} else if (type == PrimitiveType::Triangle) { //!< 三角形
		primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		return;
	}

	Assert(false); //!< 未定義のtype
}

void GraphicsPipelineDesc::SetRTVFormat(DXGI_FORMAT format) {
	rtvFormats.emplace_back(format);
	Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "RTV Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineDesc::SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]) {
	rtvFormats.insert(rtvFormats.end(), formats, formats + size);
	Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "RTV Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineDesc::SetDSVFormat(DXGI_FORMAT format) {
	dsvFormat = format;
}

void GraphicsPipelineDesc::SetViewport(const Vector2ui& size) {
	// viewportの設定
	viewport.Width    = static_cast<float>(size.x);
	viewport.Height   = static_cast<float>(size.y);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形の設定
	rect.left   = 0;
	rect.right  = size.x;
	rect.top    = 0;
	rect.bottom = size.y;
}

void GraphicsPipelineDesc::CreateDefaultDesc() {
	SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	SetElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

	SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	SetDepthStencil(true);

	SetBlendMode(0, BlendMode::kBlendModeNormal);
	SetIndependentBlendEnable(false);

	SetPrimitive(PrimitiveType::Triangle);

	SetRTVFormat(kOffscreenFormat);
	SetDSVFormat(kDefaultDepthFormat);

	SetViewport(kMainWindowSize);
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

CompileBlobCollection* GraphicsPipelineState::collection_ = nullptr;
BlendState* GraphicsPipelineState::blendState_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineState::CreateBlob(const std::wstring& filename, GraphicsShaderType type) {
	Assert(collection_ != nullptr, "collection is not set.");
	blobs_[static_cast<uint32_t>(type)].blob     = collection_->TryCreateBlob(filename, GetProfile(type));
	blobs_[static_cast<uint32_t>(type)].filename = filename;

	if (type == GraphicsShaderType::ms) {
		isUseMeshShaderPipeline_ = true;
	}
}

void GraphicsPipelineState::CreateRootSignature(Device* device, const GraphicsRootSignatureDesc& desc) {
	rootSignatureDesc_ = desc;
	rootSignatureDesc_.ShrinkToFit();

	device_ = device;

	CreateRootSignature();
}

void GraphicsPipelineState::CreatePipeline(Device* device, const GraphicsPipelineDesc& desc) {
	Assert(device_ != nullptr, "device is not set.");
	pipelineDesc_ = desc;

	device_ = device;

	CreatePipeline();
}

void GraphicsPipelineState::CheckAndUpdatePipeline() {
	if (CheckShaderHotReload()) {
		CreatePipeline();
	}
}

void GraphicsPipelineState::Term() {
}

void GraphicsPipelineState::HotReloadShader() {
	Assert(collection_ != nullptr, "collection is not set.");

	for (uint32_t i = 0; i < blobs_.size(); ++i) {
		if (blobs_[i].blob.has_value()) {
			collection_->HotReload(blobs_[i].filename);
		}
	}

	CheckAndUpdatePipeline();
}

void GraphicsPipelineState::SetPipeline(CommandContext* context) const {
	SetPipeline(context->GetCommandList());
}

void GraphicsPipelineState::SetPipeline(ID3D12GraphicsCommandList* commandList) const {
	commandList->RSSetViewports(1, &pipelineDesc_.viewport);
	commandList->RSSetScissorRects(1, &pipelineDesc_.rect);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipeline_.Get());

	if (!isUseMeshShaderPipeline_) {
		//!< vertex pipeline のときIAPrimitiveが必要になるので
		commandList->IASetPrimitiveTopology(pipelineDesc_.primitiveTopology);
	}
	
}

void GraphicsPipelineState::SetExternal(CompileBlobCollection* collection, BlendState* blendState) {
	collection_ = collection;
	blendState_ = blendState;
}

CompileProfile GraphicsPipelineState::GetProfile(GraphicsShaderType type) {
	switch (type) {
		case GraphicsShaderType::vs:
			return CompileProfile::vs;
			break;

		case GraphicsShaderType::gs:
			return CompileProfile::gs;
			break;

		case GraphicsShaderType::ms:
			return CompileProfile::ms;
			break;
		case GraphicsShaderType::as:
			return CompileProfile::as;
			break;

		case GraphicsShaderType::ps:
			return CompileProfile::ps;
			break;
	}

	Assert(false, "Graphic Shader Type is not undefine.");
	return {};
}

void GraphicsPipelineState::CreateRootSignature() {
	Assert(device_ != nullptr, "device is not set.");
	rootSignature_ = rootSignatureDesc_.CreateRootSignature(device_->GetDevice());
}

void GraphicsPipelineState::CreatePipeline() {
	Assert(device_ != nullptr, "device is not set.");

	if (isUseMeshShaderPipeline_) {

		//!< mesh piplineの設定
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature     = rootSignature_.Get();
		desc.RasterizerState    = pipelineDesc_.rasterizerDesc;
		//desc.BlendState         = blendState_->operator[](pipelineDesc_.blendMode);
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

		auto hr = device_->GetDevice()->CreatePipelineState(
			&descStream,
			IID_PPV_ARGS(&pipeline_)
		);

		Assert(SUCCEEDED(hr));

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
		auto hr = device_->GetDevice()->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(&pipeline_)
		);

		Assert(SUCCEEDED(hr));
	}
}

D3D12_SHADER_BYTECODE GraphicsPipelineState::GetBytecode(GraphicsShaderType type, bool isRequired) {
	if (!blobs_[static_cast<uint32_t>(type)].blob.has_value()) {
		if (isRequired) {
			Assert(false, "Required Blob is not set.");
		}

		return {};
	}

	if (blobs_[static_cast<uint32_t>(type)].blob.value().expired()) {
		Assert(collection_ != nullptr, "collection is not set.");
		blobs_[static_cast<uint32_t>(type)].blob = collection_->GetBlob(blobs_[static_cast<uint32_t>(type)].filename);
	}

	auto blob = blobs_[static_cast<uint32_t>(type)].blob.value().lock();

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = (*blob)->GetBufferSize();
	bytecode.pShaderBytecode = (*blob)->GetBufferPointer();

	return bytecode;
}

D3D12_BLEND_DESC GraphicsPipelineState::GetBlendDesc() const {
	D3D12_BLEND_DESC desc = {};
	desc.IndependentBlendEnable = pipelineDesc_.isIndependentBlendEnable;
	desc.RenderTarget[0]        = blendState_->GetDesc(pipelineDesc_.blendModes[0]);

	if (desc.IndependentBlendEnable) {
		for (uint32_t i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			desc.RenderTarget[i] = blendState_->GetDesc(pipelineDesc_.blendModes[i]);
		}
	}

	return desc;
}

bool GraphicsPipelineState::CheckShaderHotReload() const {
	for (const auto& blob : blobs_) {
		if (blob.blob.has_value() && blob.blob.value().expired()) {
			return true;
		}
	}

	return false;
}
