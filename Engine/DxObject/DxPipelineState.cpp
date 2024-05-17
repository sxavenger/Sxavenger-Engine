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

#include "externals/DirectXTex/d3dx12.h"

namespace {
	struct MeshShaderPipelineStateDesc {
		ID3D12RootSignature* pRootSignature;

		D3D12_SHADER_BYTECODE as;
		D3D12_SHADER_BYTECODE ms;
		D3D12_SHADER_BYTECODE ps;

		D3D12_BLEND_DESC         blend;
		D3D12_RASTERIZER_DESC    rasterizer;
		D3D12_DEPTH_STENCIL_DESC depth;

		UINT             sampleMask;
		DXGI_SAMPLE_DESC sampleDesc;

		D3D12_RT_FORMAT_ARRAY rtFormats;
		DXGI_FORMAT           dsFormat;

		D3D12_PIPELINE_STATE_FLAGS flags;
	};
}

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
	{
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

	}
}

void DxObject::PipelineState::Term() {
}