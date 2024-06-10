#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <vector>
#include <variant>
#include <string>
#include <numeric>

// ComPtr
#include <ComPtr.h>

// dxrObject
#include <DxrMethod.h>

// dxObject
#include <DxBufferResource.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject nemapspace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	//-----------------------------------------------------------------------------------------
	// using GPUBuffer
	//-----------------------------------------------------------------------------------------
	using GPUHandle         = D3D12_GPU_DESCRIPTOR_HANDLE;                                            //!< handle型
	using GPUVirtualAddress = std::variant<D3D12_GPU_VIRTUAL_ADDRESS, DxObject::BaseBufferResource*>; //!< virtualAddress型
	using GPUBuffer         = std::variant<GPUHandle, GPUVirtualAddress>;                             //!< buffer型

	////////////////////////////////////////////////////////////////////////////////////////////
	// RecordBuffer class
	////////////////////////////////////////////////////////////////////////////////////////////
	class RecordBuffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~RecordBuffer() { Term(); }

		void Term();

		void SetHandle(
			uint32_t index,
			const GPUHandle& handle
		);

		void SetAddress(
			uint32_t index,
			const GPUVirtualAddress& address
		);

		void SetExport(
			ExportType type, const std::wstring& exportName
		);

		const size_t GetRecordSize() const {
			return std::reduce(recordSizes_.begin(), recordSizes_.end());
		}

		const std::wstring& GetExport() const { return export_; }

		const std::vector<GPUBuffer>& GetRecordOrder() const { return recordOrder_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ExportType type_;
		std::wstring export_ = L"";
		/*
		 raygeneration, miss -> シェーダー関数名
		 hitgroup            -> hitgroup名
		*/

		// localRootSignatureの順番で保存
		std::vector<GPUBuffer> recordOrder_;
		std::vector<size_t>    recordSizes_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void AutoResize(uint32_t index);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// constexpr
	////////////////////////////////////////////////////////////////////////////////////////////

	constexpr const size_t kGPUDescriptorHandle = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
	constexpr const size_t kGPUVirtualAddress   = sizeof(D3D12_GPU_VIRTUAL_ADDRESS);

}