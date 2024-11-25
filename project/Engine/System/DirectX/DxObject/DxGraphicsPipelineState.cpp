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
	if (type == PrimitiveType::kLine) { //!< 線分
		primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return;

	} else if (type == PrimitiveType::kTriangle) { //!< 三角形
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

void GraphicsPipelineDesc::CreateDefaultDesc() {
	SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	SetElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

	SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	SetDepthStencil(true);

	SetBlendMode(0, BlendMode::kBlendModeNormal);
	SetIndependentBlendEnable(false);

	SetPrimitive(PrimitiveType::kTriangle);

	SetRTVFormat(kOffscreenFormat);
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

CompileBlobCollection* GraphicsPipelineState::collection_ = nullptr;
BlendState* GraphicsPipelineState::blendState_            = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineState::Term() {
}

void GraphicsPipelineState::CreateBlob(const std::wstring& filename, GraphicsShaderType type) {
	Assert(collection_ != nullptr, "collection is not set.");
	blobs_[static_cast<uint32_t>(type)].blob     = collection_->TryCreateBlob(filename, ToProfile(type));
	blobs_[static_cast<uint32_t>(type)].filename = filename;

	if (type == GraphicsShaderType::ms) {
		isUseMeshShaderPipeline_ = true;
	}
}

void GraphicsPipelineState::CreateRootSignature(Device* device, GraphicsRootSignatureDesc& desc) {
	rootSignatureDesc_ = std::move(desc);
	rootSignatureDesc_.ShrinkToFit();

	device_ = device;

	CreateRootSignature();
}

void GraphicsPipelineState::CreatePipeline(Device* device, const GraphicsPipelineDesc& desc) {
	pipelineDesc_ = desc;
	device_       = device;

	CreatePipeline();
}

void GraphicsPipelineState::ReloadShader() {
	Assert(collection_ != nullptr, "collection is not set.");

	for (uint32_t i = 0; i < blobs_.size(); ++i) {
		if (blobs_[i].blob.has_value()) {
			collection_->Reload(blobs_[i].filename);
		}
	}

	CheckAndUpdatePipeline();
}

void GraphicsPipelineState::CheckAndUpdatePipeline() {
	if (CheckShaderReloadStatus()) {
		CreatePipeline();
	}
}

void GraphicsPipelineState::SetPipeline(ID3D12GraphicsCommandList* commandList, const Vector2ui& windowSize) const {

	// viewportの設定
	D3D12_VIEWPORT viewport = {};
	viewport.Width    = static_cast<float>(windowSize.x);
	viewport.Height   = static_cast<float>(windowSize.y);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形の設定
	D3D12_RECT rect = {};
	rect.left   = 0;
	rect.right  = windowSize.x;
	rect.top    = 0;
	rect.bottom = windowSize.y;

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
	SetPipeline(context->GetCommandList(), windowSize);
}

void GraphicsPipelineState::ReloadAndSetPipeline(ID3D12GraphicsCommandList* commandList, const Vector2ui& windowSize) {
	CheckAndUpdatePipeline();
	SetPipeline(commandList, windowSize);
}

void GraphicsPipelineState::ReloadAndSetPipeline(CommandContext* context, const Vector2ui& windowSize) {
	ReloadAndSetPipeline(context->GetCommandList(), windowSize);
}

void GraphicsPipelineState::SetExternal(CompileBlobCollection* collection, BlendState* blendState) {
	collection_ = collection;
	blendState_ = blendState;
}


void GraphicsPipelineState::CreateRootSignature() {
	Assert(device_ != nullptr, "device is not set.");
	rootSignature_ = rootSignatureDesc_.CreateGraphicsRootSignature(device_->GetDevice());
}

void GraphicsPipelineState::CreatePipeline() {
	Assert(device_ != nullptr, "device is not set.");

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

IDxcBlob* GraphicsPipelineState::GetBlob(GraphicsShaderType type) {
	if (!blobs_[static_cast<uint32_t>(type)].blob.has_value()) {
		Assert(false, "blob is not set.");  //!< blobが設定されていない
		return {};
	}

	if (blobs_[static_cast<uint32_t>(type)].blob.value().expired()) {
		//!< hotReloadされている場合, 再度取得
		Assert(collection_ != nullptr, "external collection is not set."); //!< external collectionが設定されていない
		blobs_[static_cast<uint32_t>(type)].blob = collection_->GetBlob(blobs_[static_cast<uint32_t>(type)].filename);
	}

	auto blob = blobs_[static_cast<uint32_t>(type)].blob.value().lock();

	return (*blob).Get();
}

D3D12_SHADER_BYTECODE GraphicsPipelineState::GetBytecode(GraphicsShaderType type, bool isRequired) {
	if (!blobs_[static_cast<uint32_t>(type)].blob.has_value()) {
		Assert(!isRequired, "required blob is not set.");  //!< 絶対的に使用されるblobが設定されていない.
		return {};
	}

	IDxcBlob* blob = GetBlob(type);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = blob->GetBufferSize();
	bytecode.pShaderBytecode = blob->GetBufferPointer();

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

bool GraphicsPipelineState::CheckShaderReloadStatus() {
	for (const auto& blob : blobs_) {
		if (blob.blob.has_value() && blob.blob.value().expired()) {
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionGraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ReflectionGraphicsPipelineState::ReflectionRootSignature(Device* device) {
	device_ = device;

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
	CreateRootSignature();
}

void ReflectionGraphicsPipelineState::BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc) {
	table_.BindGraphicsBuffer(context, desc);
}

void ReflectionGraphicsPipelineState::CheckAndUpdatePipeline() {
	if (CheckShaderReloadStatus()) {
		ReflectionRootSignature(device_);
		CreatePipeline();
	}
}

void ReflectionGraphicsPipelineState::TrySetBlobToTable(GraphicsShaderType type, ShaderVisibility visibility, bool isRequired) {
	if (!blobs_[static_cast<uint32_t>(type)].blob.has_value()) {
		Assert(!isRequired, "required blob is not set."); //!< 絶対的に使用されるblobが設定されていない.
		return; //!< blobが登録されていないのでreturn
	}

	IDxcBlob* blob = GetBlob(type);

	ComPtr<ID3D12ShaderReflection> reflection = collection_->GetCompiler()->Reflection(blob);
	table_.CreateTable(reflection.Get(), visibility);
}

