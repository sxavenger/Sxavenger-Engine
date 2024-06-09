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
#include <unordered_set>

// ComPtr
#include <ComPtr.h>

// DxrObject
#include <DxrRootSignature.h>
#include <DxrShaderBlob.h>
#include <DxrMethod.h>

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

		void SetLocalRootSignatureDesc(uint32_t index, DxObject::Devices* device, const RootSignatureDesc& desc);

		void CreateShadeBlob();

		//=========================================================================================
		// variables
		//=========================================================================================

		// rootSignature
		RootSignature* globalRootSignature; //!< 描画で使用するのでユーザー側が管理しとく必要がある

		std::vector<std::unique_ptr<LocalRootSignature>> localRootSignatures;

		std::unique_ptr<ShaderBlob> blob; // FIXME: shaderが別々であっても機能するように

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void AutoCreateLocalRootSignature(uint32_t index);

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

		ID3D12StateObject* GetStateObject() const { return stateObject_.Get(); }

		ID3D12StateObjectProperties* GetProperties() const { return properties_.Get(); }

		const uint32_t GetHitgroupCount() const { return hitgroupCount_; }

		const std::unordered_set<std::wstring>& GetExports(ExportType type) const { return exports_[type]; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		// stateObject //

		ComPtr<ID3D12StateObject>           stateObject_;
		ComPtr<ID3D12StateObjectProperties> properties_;

		// shaderTable //

		uint32_t hitgroupCount_;

		std::unordered_set<std::wstring> exports_[kNotSelectExport];

	};

}