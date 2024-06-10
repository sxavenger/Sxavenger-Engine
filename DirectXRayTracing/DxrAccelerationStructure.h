#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>
#include <DxDescriptorHeaps.h>
#include <DxStructuredBuffer.h>
#include <DxrMethod.h>

// DxrObject
#include <DxrRecordBuffer.h>

// c++
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>

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

		~BottomLevelAS() { Term(); }

		//! @brief BLASの生成(StructuredBufferが内部保持される)
		void Create(
			DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
			const std::wstring& hitgroup
		);

		//! @brief BLASの生成(外部参照)
		void Create(
			DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
			DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer,
			const std::wstring& hitgroup
		);

		void Term();

		void SetHandle(
			uint32_t index, const GPUHandle& handle) {
			recordBuffer_->SetHandle(index, handle);
		}

		void SetAddress(
			uint32_t index, const GPUVirtualAddress& address) {
			recordBuffer_->SetAddress(index, address);
		}

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffers_.asbuffer->GetGPUVirtualAddress(); }

		const std::wstring& GetHitgruop() const { return hitgroup_; }

		const size_t GetRecordSize() const { return recordBuffer_->GetRecordSize(); }

		RecordBuffer* GetRecordBuffer() const { return recordBuffer_.get(); }

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		AccelerationStructuredBuffers buffers_;

		//!< structuredBufferが外部参照されている場合はempty
		std::unique_ptr<DxObject::StructuredBuffer> verticesStrucuturedBuffer_;
		std::unique_ptr<DxObject::StructuredBuffer> indicesStrucuturedBuffer_;

		std::wstring hitgroup_ = L"";
		
		// recordBuffer
		std::unique_ptr<RecordBuffer> recordBuffer_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceBuffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InstanceBuffer {

		//=========================================================================================
		// methods
		//=========================================================================================

		InstanceBuffer() { Init(); }

		~InstanceBuffer() { Term(); }

		void Init();

		void Term();

		void SetInstance(
			uint32_t index,
			BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t instanceId
		);

		void Clear();

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			return descBuffer_->GetGPUVirtualAddress();
		}

		const uint32_t GetCurrentIndexSize() const { return descBuffer_->GetCurrentIndexSize(); }

		const uint32_t GetMaxIndexSize() const { return descBuffer_->GetIndexSize(); }

		//=========================================================================================
		// variables
		//=========================================================================================

		std::unique_ptr<DxObject::DynamicBufferResource<D3D12_RAYTRACING_INSTANCE_DESC>> descBuffer_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TopLevelAS class
	////////////////////////////////////////////////////////////////////////////////////////////
	class TopLevelAS {
	private:

		////////////////////////////////////////////////////////////////////////////////////////////
		// InstanceData structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct InstanceData {
			BottomLevelAS* blas;
			Matrix4x4      worldMatrix;
			uint32_t       instanceId;
			// uint32_t hitgroupIndex
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		TopLevelAS() { Init(); }

		~TopLevelAS() { Term(); }

		void Init();

		void Term();
		
		void StartBlasSetup();

		void EndBlasSetup();

		void SetBLAS(
			BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t instanceId
		);

		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUDescriptorHandle() const { return descriptor_.handleGPU; }

		const std::vector<InstanceData>& GetInstances() const { return instances_; }

		const size_t GetTopRecordSize() const { return topRecordSize_; }

		const uint32_t GetObjectSize() const { return static_cast<uint32_t>(instances_.size()); }

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		// やってることはStructredBufferだが今のDxStructredBufferとdesc設定が違うため手動設定
		AccelerationStructuredBuffers buffers_;
		DxObject::Descriptor descriptor_;

		std::vector<InstanceData>       instances_;
		std::unique_ptr<InstanceBuffer> instanceBuffer_;

		size_t topRecordSize_ = 0;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void SetInstanceBuffer();

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