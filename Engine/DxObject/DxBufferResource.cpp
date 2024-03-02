#include <DxBufferResource.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BufferIndex class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::BufferIndex::Init(uint32_t indexSize) {
	// indexSizeの確認
	if (indexSize <= 0) {
		assert(false); //!< indexが0以下は配列にできない
		return;
	}

	// indexSizeの保存
	indexSize_ = indexSize;
}

void DxObject::BufferIndex::Term() {
	indexSize_ = NULL;
}

bool DxObject::BufferIndex::CheckIndex(uint32_t index) {
	if (index > indexSize_ - 1) {
		assert(false); //!< indexがsize以上
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// IndexBufferResource methods
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::IndexBufferResource::IndexBufferResource(DxObject::Devices* devices, const BufferIndex* vertexBuffer)
	: DxObject::BufferIndex() {

	if (vertexBuffer->GetSize() < 3) {
		assert(false); //!< 三角形に必要なindexSize以下
	}

	indexSize_ = (vertexBuffer->GetSize() - 2) * 3;
	
	Init(devices);
}

void DxObject::IndexBufferResource::Init(DxObject::Devices* devices) {

	// kMaxTrinagleCountの確認
	if (indexSize_ % 3 != 0) {
		assert(false); // 三角形index分ではない
	}

	kMaxTriangleCount_ = indexSize_ / 3;

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		sizeof(uint32_t) * indexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));
}

void DxObject::IndexBufferResource::Term() {
	resource_->Release();
}
