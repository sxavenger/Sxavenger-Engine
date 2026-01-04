#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignatureDesc.h"
#include "DxrBindBuffer.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class
////////////////////////////////////////////////////////////////////////////////////////////
class LocalRootSignature {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LocalRootSignature()  = default;
	~LocalRootSignature() = default;

	void CreateRootSignature(DxObject::Device* device, const LocalRootSignatureDesc& desc);

	//* getter *//

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

	size_t GetBufferStride() const { return stride_; }

	const WriteBindBufferTable& GetTable() const { return table_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* root signature *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//* size *//

	size_t stride_ = NULL;

	//* table *//

	WriteBindBufferTable table_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateTable(const LocalRootSignatureDesc& desc);

};

DXROBJECT_NAMESPACE_END
