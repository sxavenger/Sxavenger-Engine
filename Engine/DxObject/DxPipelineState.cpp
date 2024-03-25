#include "DxPipelineState.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>
#include <DxCommand.h>
#include <DxShaderBlob.h>
#include <DxRootSignature.h>

#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PipelineState methods
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::PipelineState::PipelineState(
	Devices* devices, ShaderBlob* shaderBlob, RootSignature* rootSignature, const D3D12_BLEND_DESC& blendDesc) {

	Init(devices, shaderBlob, rootSignature, blendDesc);
}

DxObject::PipelineState::~PipelineState() { Term(); }

void DxObject::PipelineState::Init(
	Devices* devices, ShaderBlob* shaderBlob, RootSignature* rootSignature, const D3D12_BLEND_DESC& blendDesc) {

	// deviceの取り出し
	ID3D12Device* device = devices->GetDevice();

	// inputLayoutの設定 TODO: inputLayout class
	D3D12_INPUT_ELEMENT_DESC descIE[3] = {};
	descIE[0].SemanticName      = "POSITION";
	descIE[0].SemanticIndex     = 0;
	descIE[0].Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descIE[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	descIE[1].SemanticName      = "TEXCOORD";
	descIE[1].SemanticIndex     = 0;
	descIE[1].Format            = DXGI_FORMAT_R32G32_FLOAT;
	descIE[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	descIE[2].SemanticName      = "NORMAL";
	descIE[2].SemanticIndex     = 0;
	descIE[2].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
	descIE[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC descIL = {};
	descIL.pInputElementDescs = descIE;
	descIL.NumElements        = _countof(descIE);

	// RasterizerStateの設定 TODO: rastarizer state class
	D3D12_RASTERIZER_DESC descR = {};
	descR.CullMode = D3D12_CULL_MODE_BACK;
	descR.FillMode = D3D12_FILL_MODE_SOLID;

	// shaderBlobの取り出し
	IDxcBlob* blob_VS = shaderBlob->GetShaderBlob_VS();
	IDxcBlob* blob_PS = shaderBlob->GetShaderBlob_PS();
	IDxcBlob* blob_GS = shaderBlob->GetShaderBlob_GS();

	// DepthStensilStateの設定 TODO: depthStencil class
	D3D12_DEPTH_STENCIL_DESC descDS = {};
	descDS.DepthEnable    = true;
	descDS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// PipelineStateの生成
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature        = rootSignature->GetRootSignature();
		desc.InputLayout           = descIL;
		desc.VS                    = { blob_VS->GetBufferPointer(), blob_VS->GetBufferSize() };
		desc.PS                    = { blob_PS->GetBufferPointer(), blob_PS->GetBufferSize() };
		desc.BlendState            = blendDesc;
		desc.RasterizerState       = descR;
		desc.NumRenderTargets      = 1;
		desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.SampleDesc.Count      = 1;
		desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
		desc.DepthStencilState     = descDS;
		desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if (blob_GS != nullptr) { //!< GSがある場合
			desc.GS = { blob_GS->GetBufferPointer(), blob_GS->GetBufferSize() };
		}

		auto hr = device->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(&graphicsPipelineState_)
		);

		assert(SUCCEEDED(hr));
	}
}

void DxObject::PipelineState::Term() {
}