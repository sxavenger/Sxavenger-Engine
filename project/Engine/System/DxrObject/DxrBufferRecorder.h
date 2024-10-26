#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrBufferTypeContainer.h>
#include <Engine/System/DxrObject/DxrExportGroup.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BufferRecorder class
////////////////////////////////////////////////////////////////////////////////////////////
class BufferRecoreder {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BufferRecoreder()  = default;
	~BufferRecoreder() { Term(); }

	void Create(const ExportGroup* exportGroup);

	void Term();

	void SetAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS& address);

	void SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	//* getter *//

	const ExportGroup* GetExportGroup() const { return exportGroup_; }

	const std::vector<GPUBuffer>& GetRecorder() const { return recorder_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const ExportGroup* exportGroup_ = nullptr;

	//* recorder *//

	std::vector<GPUBuffer> recorder_;

};

_DXROBJECT_NAMESPACE_END