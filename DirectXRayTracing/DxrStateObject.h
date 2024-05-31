#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>

// ComPtr
#include <ComPtr.h>

// DxrObject
#include <DxrRootSignature.h>
#include <DxrShaderBlob.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//-----------------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------------------- 
namespace DxObject {
	class Devices;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// StateObjectDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct StateObjectDesc {

		//=========================================================================================
		// methods
		//=========================================================================================

		~StateObjectDesc() { Clear(); }

		void Clear();

		void CreateLocalRootSignature(uint32_t size);
		void SetLocalRootSignatureDesc(uint32_t index, DxObject::Devices* device, const RootSignatureDesc& desc);

		void CreateShadeBlob();

		//=========================================================================================
		// variables
		//=========================================================================================

		// rootSignature
		RootSignature* globalRootSignature; // ユーザー側が管理しとく必要がある
		std::vector<std::unique_ptr<LocalRootSignature>> localRootSignatures;

		std::unique_ptr<ShaderBlob> blob;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// StateObject class
	////////////////////////////////////////////////////////////////////////////////////////////
	class StateObject {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(DxObject::Devices* devices, const StateObjectDesc& descs);

		void Term();

		const ComPtr<ID3D12StateObject>& GetComPtrStateObject() { return stateObject_; }

		ID3D12StateObject* GetStateObject() const { return stateObject_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		// stateObject //

		ComPtr<ID3D12StateObject> stateObject_;

		// shaderTable //



	};

}