#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// methods
#include <DxObjectMethod.h>
#include <DxGraphicsBlob.h>
#include <DxBlendState.h>

// c++
#include <cstdint>
#include <cassert>
#include <vector>

// ComPtr
#include <ComPtr.h>

// lib
#include <Environment.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class Devices;

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderStage enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ShaderStage {
		SHADER_ALL           = D3D12_SHADER_VISIBILITY_ALL,
		SHADER_VERTEX        = D3D12_SHADER_VISIBILITY_VERTEX,
		SHADER_HULL          = D3D12_SHADER_VISIBILITY_HULL,
		SHADER_DOMAIN        = D3D12_SHADER_VISIBILITY_DOMAIN,
		SHADER_GEOMETRY      = D3D12_SHADER_VISIBILITY_GEOMETRY,
		SHADER_PIXEL         = D3D12_SHADER_VISIBILITY_PIXEL,
		SHADER_AMPLIFICATION = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
		SHADER_MESH          = D3D12_SHADER_VISIBILITY_MESH
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsRootSignatureDesc class
	////////////////////////////////////////////////////////////////////////////////////////////
	class GraphicRootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~GraphicRootSignatureDesc() { Clear(); }

		void Resize(uint32_t paramSize, uint32_t samplerSize);

		void Clear();

		void SetCBV(uint32_t index, ShaderStage stage, UINT shaderRegister);

		void SetSRV(uint32_t index, ShaderStage stage, UINT shaderRegister);

		void SetSampler(uint32_t index, TextureMode mode, ShaderStage stage, UINT shaderRegister);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<D3D12_ROOT_PARAMETER>      params;
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

	private:

		//=========================================================================================
		// private varibles
		//=========================================================================================

		std::vector<D3D12_DESCRIPTOR_RANGE> ranges;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsPipeline class
	////////////////////////////////////////////////////////////////////////////////////////////
	class GraphicsPipeline {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		GraphicsPipeline() { CreateViewports(); }

		void CreateRootSiganture(Devices* devices, const GraphicRootSignatureDesc& descs);

		void CreatePipeline(Devices* devices, GraphicsBlob* graphicBlob, BlendMode blendMode);

		void SetPipeline(ID3D12GraphicsCommandList* commandList);

		static void SetBlendState(BlendState* blendState) { blendState_ = blendState; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* statics */
		static BlendState* blendState_;

		/* rootSignature */
		ComPtr<ID3D12RootSignature> rootSignature_;

		/* pipeline */
		ComPtr<ID3D12PipelineState> pipeline_;

		/* parameter */
		GraphicsBlob* blob_;

		/* viewports */
		D3D12_VIEWPORT viewport_;
		D3D12_RECT     scissorRect_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateViewports(int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	};
}