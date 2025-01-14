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

	void SetVirtualCBV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetHandleSRV(uint32_t index, uint32_t shaderRegister);
	void SetHandleUAV(uint32_t index, uint32_t shaderRegister);

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

	void SetVirtualCBV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualSRV(uint32_t index, uint32_t shaderRegister);
	void SetVirtualUAV(uint32_t index, uint32_t shaderRegister);

	//* handle *//

	void SetHandleSRV(uint32_t index, uint32_t shaderRegister);
	void SetHandleUAV(uint32_t index, uint32_t shaderRegister);

	//* root signature *//

	ComPtr<ID3D12RootSignature> CreateLocalRootSignature(ID3D12Device* device) const;

};

_DXROBJECT_NAMESPACE_END
