#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DxObject
#include <DxRootSignatureDesc.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GlobalRootSignatureDesc
	: public DxObject::CSRootSignatureDesc {
public:
};

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct LocalRootSignatureDesc
	: public GlobalRootSignatureDesc {
public:

	//=========================================================================================
	// methods
	//=========================================================================================

	LocalRootSignatureDesc() { Reserve(); }

	void Clear();

	ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device) const;

	//* virtualAddress *//

	void SetCBV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetSRV(uint32_t index, uint32_t shaderRegister);

	void SetUAV(uint32_t index, uint32_t shaderRegister);

	//* table *//

	const std::vector<GPUBuffer>& GetTable() const { return table_; }

private:

	//=========================================================================================
	// membres
	//=========================================================================================

	std::vector<GPUBuffer> table_;
	std::vector<size_t>    bufferSizes_;

	void Reserve();

	void AutoTableResize(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class
////////////////////////////////////////////////////////////////////////////////////////////
class LocalRootSignature {
public:

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LocalRootSignature() = delete;
	LocalRootSignature(const LocalRootSignatureDesc& desc) { Init(desc); }

	void Init(const LocalRootSignatureDesc& desc);

	//* Getter *//

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	const std::vector<GPUBuffer>& GetTable() const { return table_; }

	const size_t GetTotalBufferSize() const { return totalBufferSize_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rootSisnature *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//* bufferTable *//

	std::vector<GPUBuffer> table_;

	//* parameter *//

	size_t totalBufferSize_;

};

_DXROBJECT_NAMESPACE_END