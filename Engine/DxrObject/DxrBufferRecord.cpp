#include "DxrBufferRecord.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BufferRecord class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BufferRecord::Init(const ExportGroup* exportGroup) {
	// 引数の確認
	Assert(exportGroup != nullptr);

	// parameterの保存
	exportGroup_ = exportGroup;

	Clear(); //!< recorderをtableに合わせる
}

void BufferRecord::Clear() {
	if (exportGroup_->GetLocalRootSignature() != nullptr) {
		recorder_ = exportGroup_->GetLocalRootSignature()->GetTable();
	}
}

void BufferRecord::BindAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS address) {
	// recorderと型が一致するかの確認
	Assert(std::holds_alternative<GPUAddress>(recorder_.at(index))); //!< bindする型はAddressに対して, recorderはAddressではない

	// recorderにaddressの保存
	recorder_.at(index) = address;
}

void BufferRecord::BindHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE handle) {
	// recorderと型が一致するかの確認
	Assert(std::holds_alternative<GPUHandle>(recorder_.at(index))); //!< bindする型はhandleに対して, recorderはHandleではない

	// recorderにaddressの保存
	recorder_.at(index) = handle;
}
