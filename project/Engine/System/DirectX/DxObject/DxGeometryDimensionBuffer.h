#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDimensionBuffer.h"
#include "DxUnorderedDimensionBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class VertexDimensionBuffer
	: public DimensionBuffer<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// VertexUnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class VertexUnorderedDimensionBuffer
	: public UnorderedDimensionBuffer<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PointIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class PointIndexDimensionBuffer
	: public DimensionBuffer<UINT> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	UINT GetIndexCount() const;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// LineIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class LineIndexDimensionBuffer
	: public DimensionBuffer<std::array<UINT, 2>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	UINT GetIndexCount() const;

	const UINT* GetIndexData() const;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class TriangleIndexDimensionBuffer
	: public DimensionBuffer<std::array<UINT, 3>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	UINT GetIndexCount() const;

	const UINT* GetIndexData() const;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGPUVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.StrideInBytes  = static_cast<UINT>(BaseDimensionBuffer::GetStride());
	return view;
}

////////////////////////////////////////////////////////////////////////////////////////////
// VertexUnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexUnorderedDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGPUVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.StrideInBytes  = static_cast<UINT>(BaseDimensionBuffer::GetStride());
	return view;
}

DXOBJECT_NAMESPACE_END
