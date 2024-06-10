#include "DxrRecordBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RecordBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::RecordBuffer::Term() {
	recordOrder_.clear();
	recordOrder_.shrink_to_fit();

	recordSizes_.clear();
	recordOrder_.shrink_to_fit();
}

void DxrObject::RecordBuffer::SetHandle(
	uint32_t index, const GPUHandle& handle) {

	AutoResize(index);

	recordOrder_[index] = handle;
	recordSizes_[index] = kGPUDescriptorHandle;
}

void DxrObject::RecordBuffer::SetAddress(
	uint32_t index, const GPUVirtualAddress& address) {

	AutoResize(index);

	recordOrder_[index] = address;
	recordSizes_[index] = kGPUVirtualAddress;
}

void DxrObject::RecordBuffer::SetExport(
	ExportType type, const std::wstring& exportName) {

	type_ = type;
	export_ = exportName;
}

//=========================================================================================
// private methods
//=========================================================================================

void DxrObject::RecordBuffer::AutoResize(uint32_t index) {

	// indexが現在のsizeを超過してるかどうか確認
	if (index < recordOrder_.size()) {
		return; //!< size内なのでresizeの必要なし
	}

	recordOrder_.resize(index + 1);
	recordSizes_.resize(index + 1);

}