#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"
#include "DxrExportGroup.h"
#include "DxrBindBuffer.h"
#include "DxrAccelerationStructure.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxCommandContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* directx12
#include <d3dx12.h>

//* c++
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <vector>

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

	void AddExport(const DxrObject::ExportGroup* expt);

	//* parameter option *//

	void SetPayloadStride(size_t stride);

	void SetAttributeStride(size_t stride);

	void SetMaxRecursionDepth(uint8_t depth);

	//* getter *//

	size_t GetCount(ExportType type) const { return exports_[static_cast<size_t>(type)].size(); }

	const std::unordered_set<const DxrObject::ExportGroup*>& GetExports(ExportType type) const { return exports_[static_cast<size_t>(type)]; }

	size_t GetMaxStride(ExportType type) const { return maxStrides_[static_cast<size_t>(type)]; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* parameters *//

	size_t payloadStride      = NULL;
	size_t attributeStride    = NULL;
	uint8_t maxRecursionDepth = NULL;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* export parameters *//

	std::array<std::unordered_set<const DxrObject::ExportGroup*>, kExportTypeCount> exports_ = {};
	std::array<size_t, kExportTypeCount>                                            maxStrides_ = {};
	

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
	void CreateStateObject(DxObject::Device* device, StateObjectDesc&& desc);

	//* shader table option *//

	void UpdateShaderTable(
		DxObject::Device* device,
		const DxrObject::TopLevelAS* toplevelAS,
		const DxrObject::WriteBindBufferDesc* raygeneration = nullptr, const DxrObject::WriteBindBufferDesc* miss = nullptr //!< HACK: 複数設定できるようにする
	);

	//* render option *//

	void SetStateObject(DxObject::CommandContext* context) const;

	void DispatchRays(DxObject::CommandContext* context, const Vector2ui& size) const;

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
	UINT                   shaderTableStride_ = NULL;

	// fixme: shader tableとstate objectを分けるべきか？

	//* desc *//

	StateObjectDesc desc_;
	D3D12_DISPATCH_RAYS_DESC dispatchDesc_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* subobject sub methods *//

	void BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindGlobalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindExportLocalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc);
	void BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc);

	//* shader table sub methods *//

	static UINT WriteIdentifier(uint8_t* dst, const void* id);
	static UINT WriteGPUVirtualAddress(uint8_t* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	static UINT WriteGPUHandle(uint8_t* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	uint8_t* WriteExport(uint8_t* dst, UINT size, const ExportGroup* expt, const WriteBindBufferDesc* desc = nullptr);

};

_DXROBJECT_NAMESPACE_END
