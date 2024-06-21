#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// methods
#include <DxObjectMethod.h>

// c++
#include <cstdint>
#include <cassert>
#include <vector>

// ComPtr
#include <ComPtr.h>

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
	// RootSignatureDesc class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RootSignatureDesc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~RootSignatureDesc() { Clear(); }

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
	// RootSignature class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RootSignature {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		RootSignature(Devices* devices, const RootSignatureDesc& descs) {
			Init(devices, descs);
		}

		~RootSignature() { Term(); }

		void Init(Devices* devices, const RootSignatureDesc& descs);

		void Term();

		ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12RootSignature> rootSignature_;

	};

}