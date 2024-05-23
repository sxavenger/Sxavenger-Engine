#include "DxPipelineState.h"
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

#include "d3dx12.h"

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
	auto device = devices->GetDevice();

	// inputLayout
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
	D3D12_RASTERIZER_DESC descRS = {};
	descRS.CullMode = D3D12_CULL_MODE_BACK;
	descRS.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStensilStateの設定 TODO: depthStencil class
	D3D12_DEPTH_STENCIL_DESC descDS = {};
	descDS.DepthEnable    = true;
	descDS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// shaderBlobの取り出し

	IDxcBlob* blob[ShaderType::kCountOfShaderType];

	for (int i = 0; i < ShaderType::kCountOfShaderType; ++i) {
		blob[i] = shaderBlob->GetShaderBlob(static_cast<ShaderType>(i));
	}

	// 必須なshaderがコンパイルされているかの確認
	assert(blob[ShaderType::PIXEL] != nullptr);

	// PipelineStateの生成
	if (shaderBlob->IsUseMeshShaders()) { //!< mesh shader pipelineの使用

		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature     = rootSignature->GetRootSignature();
		desc.MS                 = { blob[ShaderType::MESH]->GetBufferPointer(), blob[ShaderType::MESH]->GetBufferSize()};
		desc.PS                 = { blob[ShaderType::PIXEL]->GetBufferPointer(), blob[ShaderType::PIXEL]->GetBufferSize() };
		desc.RasterizerState    = descRS;
		desc.BlendState         = blendDesc;
		desc.DepthStencilState  = descDS;
		desc.DSVFormat          = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleMask         = UINT_MAX;
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count   = 1;
		desc.NumRenderTargets   = 1;
		desc.RTVFormats[0]      = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Flags              = D3D12_PIPELINE_STATE_FLAG_NONE;

		CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(desc);

		D3D12_PIPELINE_STATE_STREAM_DESC descStream = {};
		descStream.pPipelineStateSubobjectStream = &psoStream;
		descStream.SizeInBytes                   = sizeof(psoStream);

		auto hr = device->CreatePipelineState(
			&descStream,
			IID_PPV_ARGS(&graphicsPipelineState_)
		);

		assert(SUCCEEDED(hr));
		isUseDefaultPipeline_ = false;
		return;

	} else { //!< default pipelineの使用

		// default pipelineに必須なshaderがコンパイルされているかの確認
		assert(blob[ShaderType::VERTEX] != nullptr);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature        = rootSignature->GetRootSignature();
		desc.InputLayout           = descIL;
		desc.VS                    = { blob[VERTEX]->GetBufferPointer(), blob[VERTEX]->GetBufferSize() };
		desc.PS                    = { blob[PIXEL]->GetBufferPointer(), blob[PIXEL]->GetBufferSize() };
		desc.BlendState            = blendDesc;
		desc.RasterizerState       = descRS;
		desc.NumRenderTargets      = 1;
		desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.SampleDesc.Count      = 1;
		desc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
		desc.DepthStencilState     = descDS;
		desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;

		auto hr = device->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(&graphicsPipelineState_)
		);

		assert(SUCCEEDED(hr));
		isUseDefaultPipeline_ = true;
		return;
	}
}

void DxObject::PipelineState::Term() {
}