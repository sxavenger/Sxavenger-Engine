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
	IDxcBlob* blob_MS = shaderBlob->GetShaderBlob_MS();
	IDxcBlob* blob_PS = shaderBlob->GetShaderBlob_PS();

	// DepthStensilStateの設定 TODO: depthStencil class
	D3D12_DEPTH_STENCIL_DESC descDS = {};
	descDS.DepthEnable    = true;
	descDS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// PipelineStateの生成
	{

		/*D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
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
		desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;*/

		//if (blob_GS != nullptr) { //!< GSがある場合
		//	desc.GS = { blob_GS->GetBufferPointer(), blob_GS->GetBufferSize() };
		//}

		/*MeshShaderPipelineStateDesc desc = {};
		desc.pRootSignature             = rootSignature->GetRootSignature();
		desc.ms                         = { blob_MS->GetBufferPointer(), blob_MS->GetBufferSize() };
		desc.ps                         = { blob_PS->GetBufferPointer(), blob_PS->GetBufferSize() };
		desc.rasterizer                 = descR;
		desc.blend                      = blendDesc;
		desc.depth                      = descDS;
		desc.sampleMask                 = UINT_MAX;
		desc.sampleDesc.Count           = 1;
		desc.sampleDesc.Quality         = 0;
		desc.rtFormats.NumRenderTargets = 1;
		desc.rtFormats.RTFormats[0]     = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.dsFormat                   = DXGI_FORMAT_UNKNOWN;
		desc.flags                      = D3D12_PIPELINE_STATE_FLAG_NONE;*/
		
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = rootSignature->GetRootSignature();
		desc.MS = { blob_MS->GetBufferPointer(), blob_MS->GetBufferSize() };
		desc.PS = { blob_PS->GetBufferPointer(), blob_PS->GetBufferSize() };
		desc.RasterizerState = descR;
		desc.BlendState = blendDesc;
		desc.DepthStencilState = descDS;
		desc.SampleMask = UINT_MAX;
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count = 1;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(desc);

		D3D12_PIPELINE_STATE_STREAM_DESC descStream = {};
		descStream.pPipelineStateSubobjectStream = &psoStream;
		descStream.SizeInBytes = sizeof(psoStream);

		auto hr = device->CreatePipelineState(
			&descStream,
			IID_PPV_ARGS(&graphicsPipelineState_)
		);

		assert(SUCCEEDED(hr));
	}
}

void DxObject::PipelineState::Term() {
}