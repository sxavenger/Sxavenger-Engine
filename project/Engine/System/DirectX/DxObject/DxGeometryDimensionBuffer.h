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
//! @brief 点(1頂点)プリミティブ用のインデックスバッファ
class PointIndexDimensionBuffer
	: public DimensionBuffer<UINT> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	//! @brief インデックス総数を取得する
	UINT GetIndexCount() const;

	//! @brief インデックスバッファビューを取得する
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// LineIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 線(2頂点)プリミティブ用のインデックスバッファ
class LineIndexDimensionBuffer
	: public DimensionBuffer<std::array<UINT, 2>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	//! @brief インデックス総数を取得する
	UINT GetIndexCount() const;

	//! @brief インデックスデータの先頭ポインタを取得する
	const UINT* GetIndexData() const;

	//! @brief インデックスバッファビューを取得する
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 三角形(3頂点)プリミティブ用のインデックスバッファ
class TriangleIndexDimensionBuffer
	: public DimensionBuffer<std::array<UINT, 3>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* option *//

	//! @brief インデックス総数を取得する
	UINT GetIndexCount() const;

	//! @brief インデックスデータの先頭ポインタを取得する
	const UINT* GetIndexData() const;

	//! @brief インデックスバッファビューを取得する
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
