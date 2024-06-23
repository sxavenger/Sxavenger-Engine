#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <cassert>
#include <vector>

// ComPtr
#include <ComPtr.h>

// DxObject
#include <DxObjectMethod.h>
#include <DxCSBlob.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject { //!< DxSource

	////////////////////////////////////////////////////////////////////////////////////////////
	// CSRootSignatureDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CSRootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~CSRootSignatureDesc() { Clear(); }

		void Resize(uint32_t paramSize, uint32_t samplerSize);

		void Clear();

		void SetCBV(uint32_t index, UINT shaderRegister); // virtualAddress

		void SetSRV(uint32_t index, UINT shaderRegister);

		void SetUAV(uint32_t index, UINT shaderRegister);

		void SetVirtualUAV(uint32_t index, UINT shaderRegister); // virtualAddress

		void SetVirtualSRV(uint32_t index, UINT shaderRegister); // virtualAddress

		void SetSampler(uint32_t index, TextureMode mode, UINT shaderRegister);

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
	// CSStateObject class
	////////////////////////////////////////////////////////////////////////////////////////////
	class CSPipelineState {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		CSPipelineState() {}

		~CSPipelineState() { Term(); }

		void Init(const CSRootSignatureDesc& desc, CSBlob* blob);

		void Term();

		void SetCSPipeline();

		void Dispatch(UINT threadGroupX, UINT threadGroupY, UINT threadGroupZ);

	private:

		//=========================================================================================
		// private varibles
		//=========================================================================================

		ComPtr<ID3D12RootSignature> rootSignature_;
		ComPtr<ID3D12PipelineState> pipelineState_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateRootSignature(const CSRootSignatureDesc& desc);

		void CreateStateObject(CSBlob* blob);

	};

}