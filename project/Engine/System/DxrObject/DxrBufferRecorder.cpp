#include "DxrBufferRecorder.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BufferRecorder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BufferRecoreder::Create(const ExportGroup* exportGroup) {
	// 引数の保存
	exportGroup_ = exportGroup;

	// localRootSignatureが作られていないのでrecordしない
	if (exportGroup_->GetLocalRootSignature() == nullptr) {
		return;
	}

	// 型情報を取得
	recorder_ = exportGroup_->GetLocalRootSignature()->GetBufferTypeContaniner().GetContainer();
}

void BufferRecoreder::Term() {
}

void BufferRecoreder::SetAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	Assert(index < recorder_.size(), "buffer recorder out of range.");
	Assert(std::holds_alternative<GPUAddress>(recorder_.at(index)), "buffer recorder don't match type.");
	recorder_.at(index) = address;
}

void BufferRecoreder::SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	Assert(index < recorder_.size(), "buffer recorder out of range.");
	Assert(std::holds_alternative<GPUHandle>(recorder_.at(index)), "buffer recorder don't match type.");
	recorder_.at(index) = handle;
}
