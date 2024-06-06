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
#include <memory>

// DxObject
#include <DxDevices.h>

// ComPtr
#include <ComPtr.h>

// c++
#include <cstdint>
#include <unordered_map>
#include <string>
#include <variant>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject { //!< System

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderRecordType class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ShaderRecordType {
		RAYGENERATION,
		MISS,
		HITGROUP,

		kCountOfShaderRecord
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RecordDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RecordDesc {

		//=========================================================================================
		// methods
		//=========================================================================================

		~RecordDesc() { Term(); }

		void Term();

		void SetBuffer(
			uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& recordBuffer
		);

		void SetBuffer(
			uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& recordBuffer
		);

		void SetGroup(ShaderRecordType type, const std::wstring& name);

		bool CheckParameter();

		//=========================================================================================
		// variables
		//=========================================================================================

		std::wstring group = L"";
		ShaderRecordType recordType = kCountOfShaderRecord;

		std::vector<std::variant<D3D12_GPU_VIRTUAL_ADDRESS, D3D12_GPU_DESCRIPTOR_HANDLE, bool>> recordOrder;
		//!< parameter確認用にboolを使用
		
	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		void AutoResize(uint32_t index);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RecordBuffers class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RecordBuffers {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::unordered_map<std::wstring, std::vector<std::variant<D3D12_GPU_VIRTUAL_ADDRESS, D3D12_GPU_DESCRIPTOR_HANDLE>>> recordOrders_[kCountOfShaderRecord];
		/*
		 key: シェーダーグループ名
		 - raygeneration, miss, hitgroupのどれかとなる

		 second: shaderTableに書き込むBuffer
		 - 順番はlocalRootSignature参照
		*/

		// HACK: localRootSignatureと関連性を持たせること

	};

}