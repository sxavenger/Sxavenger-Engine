#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BufferTypeContainer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BufferTypeContainer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BufferTypeContainer() { Reserve(); }

	void Clear();

	void BindAddress(uint32_t index);

	void BindHandle(uint32_t index);

	//* getter *//

	const std::vector<GPUBuffer>& GetContainer() const { return container_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<GPUBuffer> container_;
	//!< 型情報しか入らない

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Reserve();

	void AutoResize(uint32_t index);

};

_DXROBJECT_NAMESPACE_END