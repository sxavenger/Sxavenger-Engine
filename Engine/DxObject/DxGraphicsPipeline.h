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
	struct GraphicsRootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~GraphicsRootSignatureDesc() { Clear(); }

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
	// PrimitiveType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum PrimitiveType {
		PRIMITIVE_LINE,
		PRIMITIVE_TRIANGLE,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsPipelineDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicsPipelineDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		GraphicsPipelineDesc() { Init(); }

		~GraphicsPipelineDesc() { Clear(); }

		void Init();

		void Clear();

		void CreateDefaultDesc();

		/* setters */

		void SetElement(const LPCSTR& semanticName, uint32_t semanticIndex, DXGI_FORMAT format);

		void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

		void SetDepthStencil(bool depthEnable);

		void SetBlendMode(BlendMode blendMode);

		void SetPrimitive(PrimitiveType type);

		void SetRTVFormat(DXGI_FORMAT format);
		void SetRTVFormats(uint32_t size, const DXGI_FORMAT formats[]);

		/* getter */

		D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;


		//=========================================================================================
		// public variables
		//=========================================================================================

		/* descs */

		std::vector<D3D12_INPUT_ELEMENT_DESC> elements;         //!< InputLayoutDesc
		D3D12_RASTERIZER_DESC                 rasterizerDesc;   //!< RasterizerDesc
		D3D12_DEPTH_STENCIL_DESC              depthStencilDesc; //!< DepthStencilDesc

		/* param */

		BlendMode blendMode;

		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
		D3D_PRIMITIVE_TOPOLOGY        primitiveTopology;

		std::vector<DXGI_FORMAT> rtvFormats;

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

		void CreateRootSignature(Devices* devices, const GraphicsRootSignatureDesc& descs);

		/* CreatePipeline */

		void CreatePipeline(
			Devices* devices, GraphicsBlob* graphicBlob, BlendMode blendMode
		);

		void CreatePipeline(
			Devices* devices, GraphicsBlob* graphicsBlob, const GraphicsPipelineDesc& descs
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
		bool                   isUseMeshPipeline_;
		D3D_PRIMITIVE_TOPOLOGY primitiveTopology_;

		/* viewports */
		D3D12_VIEWPORT viewport_;
		D3D12_RECT     scissorRect_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateViewports(int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	};
}