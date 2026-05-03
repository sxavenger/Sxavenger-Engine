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

D3D12_RESOURCE_STATES BaseDimensionBuffer::GetDefaultState(Category category) {
	switch (category) {
		case Category::Default:
		case Category::Readback:
			return D3D12_RESOURCE_STATE_COMMON;

		case Category::Upload:
			return D3D12_RESOURCE_STATE_GENERIC_READ;

		default:
			SXAVENGER_ENGINE StreamLogger::Exception("invalid category.");
	}
}

D3D12_RESOURCE_FLAGS BaseDimensionBuffer::GetResourceFlags(Category category) {
	switch (category) {
		case Category::Default:
			return D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		case Category::Upload:
		case Category::Readback:
			return D3D12_RESOURCE_FLAG_NONE;

		default:
			SXAVENGER_ENGINE StreamLogger::Exception("invalid category.");
	}
}

void BaseDimensionBuffer::CreateBuffer(DxObject::Device* device, uint32_t size, Category category) {

	//!< 引数の保存
	size_ = size;

	//!< typeの設定
	D3D12_HEAP_TYPE type = static_cast<D3D12_HEAP_TYPE>(category);

	//!< state, flagsの設定
	D3D12_RESOURCE_STATES state = BaseDimensionBuffer::GetDefaultState(category);
	D3D12_RESOURCE_FLAGS flags  = BaseDimensionBuffer::GetResourceFlags(category);

	//!< resourceの生成
	resource_ = DxObject::Resource::CreateBuffer(
		device,
		type,
		GetByteSize(),
		flags,
		state
	);

}
