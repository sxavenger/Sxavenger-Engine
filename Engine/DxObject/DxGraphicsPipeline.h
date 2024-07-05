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
	// ShaderVisibility enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ShaderVisibility {
		VISIBILITY_ALL           = D3D12_SHADER_VISIBILITY_ALL,
		VISIBILITY_VERTEX        = D3D12_SHADER_VISIBILITY_VERTEX,
		VISIBILITY_HULL          = D3D12_SHADER_VISIBILITY_HULL,
		VISIBILITY_DOMAIN        = D3D12_SHADER_VISIBILITY_DOMAIN,
		VISIBILITY_GEOMETRY      = D3D12_SHADER_VISIBILITY_GEOMETRY,
		VISIBILITY_PIXEL         = D3D12_SHADER_VISIBILITY_PIXEL,
		VISIBILITY_AMPLIFICATION = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
		VISIBILITY_MESH          = D3D12_SHADER_VISIBILITY_MESH
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsRootSignatureDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicRootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~GraphicRootSignatureDesc() { Clear(); }

		void Resize(uint32_t paramSize, uint32_t samplerSize);

		void Clear();

		/* virtualAddress */

		void SetCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister);

		void SetVirtualSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister);

		/* handle */

		void SetSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister);

		void SetSampler(uint32_t sampleIndex, TextureMode mode, ShaderVisibility stage, UINT shaderRegister);

		void SetAnisotropicSampler(uint32_t sampleIndex, TextureMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic);

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
	// GraphicsInputLayoutDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicsInputLayoutDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~GraphicsInputLayoutDesc() { Clear(); }

		void Clear();

		void SetElement(const LPCSTR& semanticName, uint32_t semanticIndex, DXGI_FORMAT format);

		D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<D3D12_INPUT_ELEMENT_DESC> elements;

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

		/* CreateRootSignature */

		void CreateRootSignature(Devices* devices, const GraphicRootSignatureDesc& descs);

		/* CreatePipeline */

		void CreatePipeline(
			Devices* devices,
			GraphicsBlob* graphicBlob, BlendMode blendMode
		);

		void CreatePipeline(
			Devices* devices,
			GraphicsBlob* graphicBlob, const GraphicsInputLayoutDesc& layout, BlendMode blendMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		void CreatePipeline(
			Devices* devices,
			GraphicsBlob* graphicBlob, BlendMode blendMode,
			uint32_t formatSize, const DXGI_FORMAT formats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]
		);

		/* setter */

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
		D3D_PRIMITIVE_TOPOLOGY primitiveType_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		/* viewports */
		D3D12_VIEWPORT viewport_;
		D3D12_RECT     scissorRect_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateViewports(int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

		//! @brief pipeline_の生成
		void CreatePipelineState(
			ID3D12Device8* device,
			const D3D12_INPUT_LAYOUT_DESC& inputLayout, const D3D12_RASTERIZER_DESC& rasterizer, const D3D12_DEPTH_STENCIL_DESC& depthStencil,
			BlendMode blendMode,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			uint32_t formatSize = 1, const DXGI_FORMAT formats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = &defaultFormat
		);
	};
}