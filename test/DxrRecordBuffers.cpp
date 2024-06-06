#include "DxrRecordBuffers.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RecordDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::RecordDesc::Term() {
	recordOrder.clear();
	recordOrder.shrink_to_fit();
}

void DxrObject::RecordDesc::SetBuffer(
	uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& recordBuffer) {

	AutoResize(index);

	recordOrder[index] = recordBuffer;
}

void DxrObject::RecordDesc::SetBuffer(
	uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& recordBuffer) {

	AutoResize(index);

	recordOrder[index] = recordBuffer;
}

void DxrObject::RecordDesc::SetGroup(ShaderRecordType type, const std::wstring& name) {
	recordType = type;
	group = name;
}

bool DxrObject::RecordDesc::CheckParameter() {

	// groupnameの確認
	assert(!group.empty()); //!< groupnameの未入力

	// recordTypeの確認
	assert(recordType != kCountOfShaderRecord);

	// parameterの確認
	for (size_t i = 0; i < recordOrder.size(); ++i) {
		assert(!std::holds_alternative<bool>(recordOrder[i])); //!< parameterの入力忘れ, indexがおかしい
	}

	return true;
}

void DxrObject::RecordDesc::AutoResize(uint32_t index) {

	if (index < recordOrder.size()) { //!< indexが今のsizeを以内
		return;
	}

	size_t beforeSize = recordOrder.size();

	// indexが最大になるようにresize
	recordOrder.resize(index + 1);

	for (size_t i = beforeSize; i < static_cast<size_t>(index); ++i) {
		// 空いたindex分はparameter未代入確認用のboolを入れておく
		recordOrder[i] = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// RecordBuffers class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::RecordBuffers::Init() {
}


