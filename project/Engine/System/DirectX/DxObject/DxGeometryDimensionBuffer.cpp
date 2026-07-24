#include "DxGeometryDimensionBuffer.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

////////////////////////////////////////////////////////////////////////////////////////////
// PointIndexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

UINT PointIndexDimensionBuffer::GetIndexCount() const {
	return BaseDimensionBuffer::GetSize();
}

D3D12_INDEX_BUFFER_VIEW PointIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGPUVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.Format         = DXGI_FORMAT_R32_UINT;
	return view;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LineIndexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

UINT LineIndexDimensionBuffer::GetIndexCount() const {
	return BaseDimensionBuffer::GetSize() * 2;
}

const UINT* LineIndexDimensionBuffer::GetIndexData() const {
	return reinterpret_cast<const UINT*>(DimensionBuffer::GetData());
}

D3D12_INDEX_BUFFER_VIEW LineIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGPUVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.Format         = DXGI_FORMAT_R32_UINT;
	return view;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

UINT TriangleIndexDimensionBuffer::GetIndexCount() const {
	return BaseDimensionBuffer::GetSize() * 3;
}

const UINT* TriangleIndexDimensionBuffer::GetIndexData() const {
	return reinterpret_cast<const UINT*>(DimensionBuffer::GetData());
}

D3D12_INDEX_BUFFER_VIEW TriangleIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGPUVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.Format         = DXGI_FORMAT_R32_UINT;
	return view;
}

