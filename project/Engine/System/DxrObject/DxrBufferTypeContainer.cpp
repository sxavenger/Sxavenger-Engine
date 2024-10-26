#include "DxrBufferTypeContainer.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BufferTypeContainer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void BufferTypeContainer::Clear() {
	container_.clear();
}

void BufferTypeContainer::BindAddress(uint32_t index) {
	AutoResize(index);
	container_.at(index) = GPUAddress{ std::nullopt };
}

void BufferTypeContainer::BindHandle(uint32_t index) {
	AutoResize(index);
	container_.at(index) = GPUHandle{ std::nullopt };
}

void BufferTypeContainer::Reserve() {
	container_.reserve(32);
}

void BufferTypeContainer::AutoResize(uint32_t index) {
	if (index + 1 > container_.size()) { //!< indexがsizeを超過してるとき
		container_.resize(index + 1);
	}
}