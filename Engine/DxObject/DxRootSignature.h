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
		SHADER_ALL           = 0,
		SHADER_VERTEX        = 1,
		SHADER_HULL          = 2,
		SHADER_DOMAIN        = 3,
		SHADER_GEOMETRY      = 4,
		SHADER_PIXEL         = 5,
		SHADER_AMPLIFICATION = 6,
		SHADER_MESH          = 7
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureMode enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum TextureMode {
		MODE_WRAP        = 1,
		MODE_MIRROR      = 2,
		MODE_CLAMP       = 3,
		MODE_BORDER      = 4,
		MODE_MIRROR_ONCE = 5
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

		void SetTexture2D(uint32_t index, ShaderStage stage, UINT shaderRegister);

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
		ID3DBlob* signatureBlob_;
		ID3DBlob* signatureErrorBlob_;

	};

}