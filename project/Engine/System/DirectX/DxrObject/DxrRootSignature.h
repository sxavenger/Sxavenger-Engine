#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignatureDesc.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* root signature *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//* size *//

	size_t size_ = NULL;

};

_DXROBJECT_NAMESPACE_END
