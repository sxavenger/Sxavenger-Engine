#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>
#include <DxDescriptorHeaps.h>
#include <DxStructuredBuffer.h>
#include <DxrMethod.h>

// c++
#include <vector>
#include <memory>

// object
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// AccelerationStructureBuffers
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AccelerationStructuredBuffers {
		ComPtr<ID3D12Resource> scratch;
		ComPtr<ID3D12Resource> asbuffer;
		ComPtr<ID3D12Resource> update;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BottomLevelAS class
	////////////////////////////////////////////////////////////////////////////////////////////
	class BottomLevelAS {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(
			DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
			const std::wstring& hitgroup
		);

		void Term();

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffers_.asbuffer->GetGPUVirtualAddress(); }

		const DxObject::Descriptor GetIndicesDescriptor() const { return indicesStrucuturedBuffer_->GetDescriptor(); }
		const DxObject::Descriptor GetVerticesDescriptor() const { return verticesStrucuturedBuffer_->GetDescriptor(); }

		const std::wstring& GetHitgruop() const { return hitgroup_; }

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		AccelerationStructuredBuffers buffers_;

		// HACK: vertices indices とわかれてしまっている
		std::unique_ptr<DxObject::StructuredBuffer> verticesStrucuturedBuffer_;
		std::unique_ptr<DxObject::StructuredBuffer> indicesStrucuturedBuffer_;

		std::wstring hitgroup_ = L"hitGroup"; // 仮

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InstanceDesc {

		//=========================================================================================
		// methods
		//=========================================================================================

		~InstanceDesc() { Clear(); }

		void Resize(uint32_t size);

		void Set( // HACK: name
			uint32_t index,
			BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t hitGroupIndex, uint32_t instanceId
		);

		void Clear();

		//=========================================================================================
		// variables
		//=========================================================================================

		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> descs;

		std::vector<BottomLevelAS*> blasPtrs_; //!< hitgruopごとに分ける

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TopLevelAS class
	////////////////////////////////////////////////////////////////////////////////////////////
	class TopLevelAS {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const InstanceDesc& instanceDesc);

		void Term();

		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUDescriptorHandle() const { return descriptor_.handleGPU; }

		const std::vector<BottomLevelAS*>& GetBLASPtrArray() const { return blasPtrs_; }

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		// やってることはStructredBufferだが今のDxStructredBufferとdesc設定が違うため手動設定
		AccelerationStructuredBuffers buffers_;
		DxObject::Descriptor descriptor_;

		std::unique_ptr<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>> instanceDescBuffer_;

		std::vector<BottomLevelAS*> blasPtrs_;

	};


}

////////////////////////////////////////////////////////////////////////////////////////////
// namespace DxrMethod
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrMethod {

	DxrObject::AccelerationStructuredBuffers CreateAccelerationStructure(
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc
	);

}