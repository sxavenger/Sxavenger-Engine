#include "DxDimensionBuffer.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDimensionBuffer::Reset() {
	resource_.Reset();
	size_ = NULL;
}

void BaseDimensionBuffer::CreateBuffer(DxObject::Device* device, uint32_t size, Category category) {

	//!< 引数の保存
	size_ = size;

	//!< typeの設定
	D3D12_HEAP_TYPE type = static_cast<D3D12_HEAP_TYPE>(category);

	//!< state, flagsの設定
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	D3D12_RESOURCE_FLAGS flags  = D3D12_RESOURCE_FLAG_NONE;

	switch (category) {
		case Category::Default:
			state = D3D12_RESOURCE_STATE_COMMON;
			flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			break;

		case Category::Upload:
			state = D3D12_RESOURCE_STATE_GENERIC_READ;
			flags = D3D12_RESOURCE_FLAG_NONE;
			break;

		case Category::Readback:
			state = D3D12_RESOURCE_STATE_COPY_DEST;
			flags = D3D12_RESOURCE_FLAG_NONE;
			break;
	}

	//!< resourceの生成
	resource_ = DxObject::Resource::CreateBuffer(
		device,
		type,
		GetByteSize(),
		flags,
		state
	);

}
