#pragma	once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* c++
#include <variant>
#include <vector>

//* DxrObject
#include <DxrExportGroup.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BufferRecord class
////////////////////////////////////////////////////////////////////////////////////////////
class BufferRecord {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BufferRecord() = delete;
	BufferRecord(const ExportGroup* exportGroup) { Init(exportGroup); }

	void Init(const ExportGroup* exportGroup);

	//* recorder option *//

	void Clear();

	void BindAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS address);
	void BindHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE handle);

	//* getter *//

	const std::vector<GPUBuffer>& GetRecorder() const { return recorder_; }

	const size_t GetTotalBufferSize() const { return exportGroup_->GetLocalRootSignature()->GetTotalBufferSize(); }

	//* externals

	const ExportGroup* GetExportGroup() const { return exportGroup_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const ExportGroup* exportGroup_ = nullptr;

	//* recorder *//

	std::vector<GPUBuffer> recorder_; //!< bufferの記録

};

_DXROBJECT_NAMESPACE_END