#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"
#include "DxrRaytracingBlob.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>

//* directx12
#include <d3dx12.h>

//* c++
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct StateObjectDesc {
public:


	//=========================================================================================
	// public methods
	//=========================================================================================

	StateObjectDesc()  = default;
	~StateObjectDesc() = default;

	//* blob option *//

	void SetBlob(const DxrObject::RaytracingBlob* blob);

	void SetExportParameter();

	//* parameter option *//

	void SetPayloadStride(size_t stride);

	void SetAttributeSize(size_t stride);

	void SetMaxRecursionDepth(uint8_t depth);

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* blob container *//

	std::unordered_set<const DxrObject::RaytracingBlob*> blobs;

	//* parameters *//

	size_t payloadStride      = NULL;
	size_t attributeStride    = NULL;
	uint8_t maxRecursionDepth = NULL;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export parameters *//

	std::array<size_t, kExportTypeCount> counts_;
	std::array<size_t, kExportTypeCount> maxStrides_;
	

};

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectContext class
////////////////////////////////////////////////////////////////////////////////////////////
class StateObjectContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StateObjectContext()  = default;
	~StateObjectContext() = default;

	//* root signature option *//

	void CreateRootSignature(DxObject::Device* device, const GlobalRootSignatureDesc& desc);

	//* state object option *//

	void CreateStateObject(DxObject::Device* device, const StateObjectDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* root signature *//

	ComPtr<ID3D12RootSignature> rootSignature_; //!< global root signature

	//* state object *//

	ComPtr<ID3D12StateObject>           stateObject_;
	ComPtr<ID3D12StateObjectProperties> properties_;

	//* shader table *//

	ComPtr<ID3D12Resource> shaderTable_;

	//* desc *//

	StateObjectDesc desc_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindGlobalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindExportLocalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc);

};

_DXROBJECT_NAMESPACE_END
