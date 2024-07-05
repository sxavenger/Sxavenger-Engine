#include "DxGraphicsPipeline.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>

// DirectxX12
#include <d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicRootSignatureDesc::Resize(uint32_t paramSize, uint32_t samplerSize) {
	params.resize(paramSize);
	ranges.resize(paramSize);
	samplers.resize(samplerSize);
}

void DxObject::GraphicRootSignatureDesc::Clear() {
	params.clear();
	params.shrink_to_fit();
	samplers.clear();
	samplers.shrink_to_fit();
	ranges.clear();
	ranges.shrink_to_fit();
}

void DxObject::GraphicRootSignatureDesc::SetCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicRootSignatureDesc::SetVirtualSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicRootSignatureDesc::SetSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister) {
	ranges[index].BaseShaderRegister                = shaderRegister;
	ranges[index].NumDescriptors                    = 1;
	ranges[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].DescriptorTable.pDescriptorRanges   = &ranges[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicRootSignatureDesc::SetSampler(uint32_t sampleIndex, TextureMode mode, ShaderVisibility stage, UINT shaderRegister) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

void DxObject::GraphicRootSignatureDesc::SetAnisotropicSampler(
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
// GraphicsInputLayoutDesc structure
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsInputLayoutDesc::Clear() {
	elements.clear();
	elements.shrink_to_fit();
}

void DxObject::GraphicsInputLayoutDesc::SetElement(const LPCSTR& semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {

	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = semanticName;
	element.SemanticIndex     = semanticIndex;
	element.Format            = format;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	elements.push_back(element);

}

D3D12_INPUT_LAYOUT_DESC DxObject::GraphicsInputLayoutDesc::GetInputLayout() const {

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

void DxObject::GraphicsPipeline::CreateRootSignature(Devices* devices, const GraphicRootSignatureDesc& descs) {

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

	if (graphicBlob != nullptr) {
		// 引数の保存
		blob_ = graphicBlob;
	}

	// menberの確認
	assert(blob_ != nullptr);
	assert(rootSignature_ != nullptr);

	// inputLayoutの設定 default
	D3D12_INPUT_ELEMENT_DESC descInputElement[3] = {};
	descInputElement[0].SemanticName      = "POSITION";
	descInputElement[0].SemanticIndex     = 0;
	descInputElement[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descInputElement[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	descInputElement[1].SemanticName      = "TEXCOORD";
	descInputElement[1].SemanticIndex     = 0;
	descInputElement[1].Format            = DXGI_FORMAT_R32G32_FLOAT;
	descInputElement[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	descInputElement[2].SemanticName      = "NORMAL";
	descInputElement[2].SemanticIndex     = 0;
	descInputElement[2].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
	descInputElement[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC descInputLayout = {};
	descInputLayout.pInputElementDescs = descInputElement;
	descInputLayout.NumElements        = _countof(descInputElement);

	// rasterizerの設定 default
	// TODO: Rasterizer structure
	D3D12_RASTERIZER_DESC descRasterizer = {};
	descRasterizer.CullMode = D3D12_CULL_MODE_BACK;
	descRasterizer.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStensilStateの設定 default
	// TODO: depthStencil structure
	D3D12_DEPTH_STENCIL_DESC descDepthStencil = {};
	descDepthStencil.DepthEnable    = true;
	descDepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CreatePipelineState(
		devices->GetDevice(),
		descInputLayout, descRasterizer, descDepthStencil,
		blendMode
	);
}

void DxObject::GraphicsPipeline::CreatePipeline(
	Devices* devices,
	GraphicsBlob* graphicBlob, const GraphicsInputLayoutDesc& layout, BlendMode blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {


	if (graphicBlob != nullptr) {
		// 引数の保存
		blob_ = graphicBlob;
	}

	// menberの確認
	assert(blob_ != nullptr);
	assert(rootSignature_ != nullptr);

	// rasterizerの設定
	D3D12_RASTERIZER_DESC descRasterizer = {};
	descRasterizer.CullMode = D3D12_CULL_MODE_BACK;
	descRasterizer.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStensilStateの設定
	D3D12_DEPTH_STENCIL_DESC descDepthStencil = {};
	descDepthStencil.DepthEnable    = true;
	descDepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CreatePipelineState(
		devices->GetDevice(),
		layout.GetInputLayout(), descRasterizer, descDepthStencil,
		blendMode,
		type
	);
}

void DxObject::GraphicsPipeline::CreatePipeline(
	Devices* devices,
	GraphicsBlob* graphicBlob, BlendMode blendMode,
	uint32_t formatSize, const DXGI_FORMAT formats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {

	if (graphicBlob != nullptr) {
		// 引数の保存
		blob_ = graphicBlob;
	}

	// menberの確認
	assert(blob_ != nullptr);
	assert(rootSignature_ != nullptr);

	// inputLayoutの設定 default
	D3D12_INPUT_ELEMENT_DESC descInputElement[3] = {};
	descInputElement[0].SemanticName      = "POSITION";
	descInputElement[0].SemanticIndex     = 0;
	descInputElement[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descInputElement[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	descInputElement[1].SemanticName      = "TEXCOORD";
	descInputElement[1].SemanticIndex     = 0;
	descInputElement[1].Format            = DXGI_FORMAT_R32G32_FLOAT;
	descInputElement[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	descInputElement[2].SemanticName      = "NORMAL";
	descInputElement[2].SemanticIndex     = 0;
	descInputElement[2].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
	descInputElement[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC descInputLayout = {};
	descInputLayout.pInputElementDescs = descInputElement;
	descInputLayout.NumElements        = _countof(descInputElement);

	// rasterizerの設定 default
	// TODO: Rasterizer structure
	D3D12_RASTERIZER_DESC descRasterizer = {};
	descRasterizer.CullMode = D3D12_CULL_MODE_BACK;
	descRasterizer.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStensilStateの設定 default
	// TODO: depthStencil structure
	D3D12_DEPTH_STENCIL_DESC descDepthStencil = {};
	descDepthStencil.DepthEnable    = true;
	descDepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDepthStencil.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CreatePipelineState(
		devices->GetDevice(),
		descInputLayout, descRasterizer, descDepthStencil,
		blendMode,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		formatSize, formats
	);
}

void DxObject::GraphicsPipeline::SetPipeline(ID3D12GraphicsCommandList* commandList) {
	
	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipeline_.Get());

	if (!blob_->IsUseMeshPipeline()) {
		//!< vertex pipeline のときIAPrimitiveが必要になるので
		commandList->IASetPrimitiveTopology(primitiveType_); //!< line描画の時の調整
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

void DxObject::GraphicsPipeline::CreatePipelineState(
	ID3D12Device8* device,
	const D3D12_INPUT_LAYOUT_DESC& inputLayout, const D3D12_RASTERIZER_DESC& rasterizer, const D3D12_DEPTH_STENCIL_DESC& depthStencil,
	BlendMode blendMode,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE type,
	uint32_t formatSize, const DXGI_FORMAT formats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {

	// blobsの取り出し
	auto& blobs = blob_->GetGraphicsBlobs();

	// Pipelineの設定
	if (blob_->IsUseMeshPipeline()) {

		//!< mesh piplineの設定
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature     = rootSignature_.Get();
		desc.RasterizerState    = rasterizer;
		desc.BlendState         = blendState_->operator[](blendMode);
		desc.DepthStencilState  = depthStencil;
		desc.DSVFormat          = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleMask         = UINT_MAX;
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count   = 1;
		desc.Flags              = D3D12_PIPELINE_STATE_FLAG_NONE;

		// rtv formatの設定
		desc.NumRenderTargets = formatSize;

		for (uint32_t i = 0; i < formatSize; ++i) {
			desc.RTVFormats[i] = formats[i];
		}

		// blobの設定
		desc.MS = { blobs[GraphicShaderType::GRAPHICS_MESH]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_MESH]->GetBufferSize() };
		desc.PS = { blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_PIXEL]->GetBufferSize() };

		if (blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION] != nullptr) {
			desc.AS = { blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION]->GetBufferPointer(), blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION]->GetBufferSize() };
		}

		// create
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
		desc.InputLayout           = inputLayout;
		desc.BlendState            = blendState_->operator[](blendMode);
		desc.RasterizerState       = rasterizer;
		desc.SampleDesc.Count      = 1;
		desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
		desc.DepthStencilState     = depthStencil;
		desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// primitiveの設定
		desc.PrimitiveTopologyType = type;

		if (type == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE) {
			primitiveType_ = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		}

		// rtv formatの設定
		desc.NumRenderTargets = formatSize;

		for (uint32_t i = 0; i < formatSize; ++i) {
			desc.RTVFormats[i] = formats[i];
		}

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