#include "DxGraphicsPipeline.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>

// DirectxX12
#include <d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsRootSignatureDesc::Resize(uint32_t paramSize, uint32_t samplerSize) {
	params.resize(paramSize);
	ranges.resize(paramSize);
	samplers.resize(samplerSize);
}

void DxObject::GraphicsRootSignatureDesc::Clear() {
	params.clear();
	params.shrink_to_fit();
	samplers.clear();
	samplers.shrink_to_fit();
	ranges.clear();
	ranges.shrink_to_fit();
}

void DxObject::GraphicsRootSignatureDesc::SetCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetVirtualSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	ranges[index].BaseShaderRegister                = shaderRegister;
	ranges[index].NumDescriptors                    = 1;
	ranges[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].DescriptorTable.pDescriptorRanges   = &ranges[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicsRootSignatureDesc::SetSampler(uint32_t sampleIndex, TextureMode mode, ShaderVisibility stage, UINT shaderRegister) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

void DxObject::GraphicsRootSignatureDesc::SetAnisotropicSampler(
	uint32_t sampleIndex, TextureMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic) {

	samplers[sampleIndex].Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers[sampleIndex].MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsPipelineDesc::Init() {
	rasterizerDesc = {};
	depthStencilDesc = {};
}

void DxObject::GraphicsPipelineDesc::Clear() {
	elements.clear();
	elements.shrink_to_fit();
}

void DxObject::GraphicsPipelineDesc::CreateDefaultDesc() {
	/* inputLayout */
	SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	SetElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

	/* rasterizer */
	SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	/* depthStencil */
	SetDepthStencil(true);

	/* blendMode */
	SetBlendMode(kBlendModeNormal);

	/* primitive */
	SetPrimitive(PRIMITIVE_TRIANGLE);

	/* rtvFormat */
	SetRTVFormat(defaultFormat);
}

void DxObject::GraphicsPipelineDesc::SetElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot) {

	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = semanticName;
	element.SemanticIndex     = semanticIndex;
	element.Format            = format;
	element.InputSlot         = inputSlot;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	elements.push_back(element);
}

void DxObject::GraphicsPipelineDesc::SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {

	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = fillMode;
}

void DxObject::GraphicsPipelineDesc::SetDepthStencil(bool depthEnable) {

	depthStencilDesc.DepthEnable    = depthEnable;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void DxObject::GraphicsPipelineDesc::SetBlendMode(BlendMode mode) {
	blendMode = mode;
}

void DxObject::GraphicsPipelineDesc::SetPrimitive(PrimitiveType type) {
	if (type == PRIMITIVE_LINE) { //!< 線分
		primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return;

	} else if (type == PRIMITIVE_TRIANGLE) { //!< 三角形
		primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		primitiveTopology     = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		return;
	}

	assert(false); //!< 未定義のtype
}

void DxObject::GraphicsPipelineDesc::SetRTVFormat(DXGI_FORMAT format) {
	rtvFormats.emplace_back(format);

	assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT); //!< RTVの設定限界
}

void DxObject::GraphicsPipelineDesc::SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]) {
	rtvFormats.insert(rtvFormats.end(), formats, formats + size);

	assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT); //!< RTVの設定限界
}

D3D12_INPUT_LAYOUT_DESC DxObject::GraphicsPipelineDesc::GetInputLayout() const {

	D3D12_INPUT_LAYOUT_DESC result = {};
	result.pInputElementDescs = elements.data();
	result.NumElements        = static_cast<UINT>(elements.size());

	return result;
}

//=========================================================================================
// static variables
//=========================================================================================

DxObject::BlendState* DxObject::GraphicsPipeline::blendState_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsPipeline::CreateRootSignature(Devices* devices, const GraphicsRootSignatureDesc& descs) {

	// deviceの取り出し
	auto device = devices->GetDevice();

	// descの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!descs.params.empty()) {
		desc.pParameters   = descs.params.data();
		desc.NumParameters = static_cast<UINT>(descs.params.size());
	}

	if (!descs.samplers.empty()) {
		desc.pStaticSamplers   = descs.samplers.data();
		desc.NumStaticSamplers = static_cast<UINT>(descs.samplers.size());
	}

	rootSignature_ = DxObjectMethod::CreateRootSignature(
		device,
		desc
	);

}

void DxObject::GraphicsPipeline::CreatePipeline(Devices* devices, GraphicsBlob* graphicBlob, BlendMode blendMode) {

	// defaultDescの生成
	GraphicsPipelineDesc desc;
	desc.CreateDefaultDesc();

	desc.blendMode = blendMode;

	CreatePipeline(
		devices, graphicBlob, desc
	);

}

void DxObject::GraphicsPipeline::CreatePipeline(
	Devices* devices, GraphicsBlob* graphicsBlob, const GraphicsPipelineDesc& descs) {

	assert((devices != nullptr) && (graphicsBlob != nullptr));

	// 引数を受け渡し
	isUseMeshPipeline_ = graphicsBlob->IsUseMeshPipeline();
	primitiveTopology_ = descs.primitiveTopology;

	// deviceの取り出し
	auto device = devices->GetDevice();

	// blobsの取り出し
	auto& blobs = graphicsBlob->GetGraphicsBlobs();

	// pipelineの設定
	if (isUseMeshPipeline_) {

		//!< mesh piplineの設定
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature     = rootSignature_.Get();
		desc.RasterizerState    = descs.rasterizerDesc;
		desc.BlendState         = blendState_->operator[](descs.blendMode);
		desc.DepthStencilState  = descs.depthStencilDesc;
		desc.DSVFormat          = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleMask         = UINT_MAX;
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count   = 1;
		desc.Flags              = D3D12_PIPELINE_STATE_FLAG_NONE;

		// RTVFormatの設定
		desc.NumRenderTargets = static_cast<UINT>(descs.rtvFormats.size());
		std::copy(descs.rtvFormats.begin(), descs.rtvFormats.begin() + descs.rtvFormats.size(), desc.RTVFormats);

		// blobの設定
		desc.MS = { blobs[GraphicShaderType::GRAPHICS_MESH]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_MESH]->GetBufferSize() };
		desc.PS = { blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferSize() };

		if (blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION] != nullptr) {
			desc.AS = { blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION]->GetBufferSize() };
		}

		// pipelineの生成
		CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(desc);

		D3D12_PIPELINE_STATE_STREAM_DESC descStream = {};
		descStream.pPipelineStateSubobjectStream = &psoStream;
		descStream.SizeInBytes                   = sizeof(psoStream);

		auto hr = device->CreatePipelineState(
			&descStream,
			IID_PPV_ARGS(&pipeline_)
		);

		assert(SUCCEEDED(hr));
		return;

	} else {

		//!< vertex pipeline設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature        = rootSignature_.Get();
		desc.InputLayout           = descs.GetInputLayout();
		desc.BlendState            = blendState_->operator[](descs.blendMode);
		desc.RasterizerState       = descs.rasterizerDesc;
		desc.SampleDesc.Count      = 1;
		desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
		desc.DepthStencilState     = descs.depthStencilDesc;
		desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.PrimitiveTopologyType = descs.primitiveTopologyType; //!< primitiveTopologyは引数の受け渡し部分で渡した

		// RTVformatの設定
		desc.NumRenderTargets = static_cast<UINT>(descs.rtvFormats.size());
		std::copy(descs.rtvFormats.begin(), descs.rtvFormats.begin() + descs.rtvFormats.size(), desc.RTVFormats);

		// blobの設定
		desc.VS = { blobs[GraphicShaderType::GRAPHICS_VERTEX]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_VERTEX]->GetBufferSize() };
		desc.PS = { blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferSize() };

		if (blobs[GraphicShaderType::GRAPHICS_GEOMETRY] != nullptr) {
			desc.GS = { blobs[GraphicShaderType::GRAPHICS_GEOMETRY]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_GEOMETRY]->GetBufferSize() };
		}

		// create
		auto hr = device->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(&pipeline_)
		);

		assert(SUCCEEDED(hr));
		return;

	}
}

void DxObject::GraphicsPipeline::SetPipeline(ID3D12GraphicsCommandList* commandList) {
	
	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipeline_.Get());

	if (!isUseMeshPipeline_) {
		//!< vertex pipeline のときIAPrimitiveが必要になるので
		commandList->IASetPrimitiveTopology(primitiveTopology_); //!< line描画の時の調整
	}
}

void DxObject::GraphicsPipeline::CreateViewports(int32_t clientWidth, int32_t clientHeight) {
	// viewportの設定
	{
		viewport_.Width    = static_cast<float>(clientWidth);
		viewport_.Height   = static_cast<float>(clientHeight);
		viewport_.TopLeftX = 0;
		viewport_.TopLeftY = 0;
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;
	}

	// シザー矩形の設定
	{
		scissorRect_.left   = 0;
		scissorRect_.right  = clientWidth;
		scissorRect_.top    = 0;
		scissorRect_.bottom = clientHeight;
	}
}