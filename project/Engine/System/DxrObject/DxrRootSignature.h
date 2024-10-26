#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrBufferTypeContainer.h>

//* DXOBJECT
#include <Engine/System/DxObject/DxRootSignatureDesc.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GlobalRootSignatureDesc
	: public DxObject::CSRootSignatureDesc {
public:

	ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device) const override;

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

	ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device* device) const override;

	//* virtualAddress *//

	void SetCBV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);

	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetSRV(uint32_t index, uint32_t shaderRegister);

	void SetUAV(uint32_t index, uint32_t shaderRegister);

	//* getter *//

	const BufferTypeContainer& GetContainer() const { return container_; }

private:

	//=========================================================================================
	// membres
	//=========================================================================================

	BufferTypeContainer container_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Reserve();

};

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class
////////////////////////////////////////////////////////////////////////////////////////////
class LocalRootSignature {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LocalRootSignature()  = default;
	~LocalRootSignature() { Term(); }

	void Create(const LocalRootSignatureDesc& desc);

	void Term();

	//* getter *//

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	const BufferTypeContainer& GetBufferTypeContaniner() const { return container_; }

	size_t GetBufferTotalSize() const { return bufferTotalSize_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* RootSignature *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//* buffer parameter *//

	BufferTypeContainer container_;
	size_t              bufferTotalSize_;
};


_DXROBJECT_NAMESPACE_END