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
	// RootSignatureDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RootSignatureDescs {
		std::vector<D3D12_ROOT_PARAMETER>      param;
		std::vector<D3D12_STATIC_SAMPLER_DESC> sampler;

		//! @breif コンストラクタ
		//! 
		//! @param[in] paramSize   paramの配列数
		//! @param[in] samplerSize samplerの配列数
		RootSignatureDescs(uint32_t paramSize, uint32_t samplerSize) {
			param.resize(paramSize);
			sampler.resize(samplerSize);
		}

		//! @brief デストラクタ
		~RootSignatureDescs() {
			param.clear();
			sampler.clear();
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RootSignature class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RootSignature {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		RootSignature(Devices* devices, const RootSignatureDescs& descs) {
			Init(devices, descs);
		}

		~RootSignature() { Term(); }

		void Init(Devices* devices, const RootSignatureDescs& descs);

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