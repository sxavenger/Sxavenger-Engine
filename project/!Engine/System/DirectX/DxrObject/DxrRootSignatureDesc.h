#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxRootSignatureDesc.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GlobalRootSignatureDesc
	: public DxObject::BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* virtual address *//

	void SetVirtualCBV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetVirtualUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* handle *//

	void SetHandleSRV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);
	void SetHandleUAV(uint32_t index, UINT shaderRegister, UINT registerSpace = 0);

	//* root signature *//

	ComPtr<ID3D12RootSignature> CreateGlobalRootSignature(ID3D12Device* device) const;

	
};

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignatureDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct LocalRootSignatureDesc
	: public DxObject::BaseRootSignatureDesc {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	//* virtual address *//

	void SetVirtualCBV(uint32_t index, UINT shaderRegister);
	void SetVirtualSRV(uint32_t index, UINT shaderRegister);
	void SetVirtualUAV(uint32_t index, UINT shaderRegister);

	//* handle *//

	void SetHandleSRV(uint32_t index, UINT shaderRegister);
	void SetHandleUAV(uint32_t index, UINT shaderRegister);

	//* root signature *//

	ComPtr<ID3D12RootSignature> CreateLocalRootSignature(ID3D12Device* device) const;

};

_DXROBJECT_NAMESPACE_END
