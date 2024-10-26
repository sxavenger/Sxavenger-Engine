#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrRootSignature.h>
#include <Engine/System/DxrObject/DxrExportGroup.h>
#include <Engine/System/DxrObject/DxrRaytracingShaderBlob.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>

//* DirectX12
#include <d3dx12.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// StateObject class
////////////////////////////////////////////////////////////////////////////////////////////
class StateObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StateObject()  = default;
	~StateObject() { Term(); }

	void Term();

	void CreateRootSignature(const GlobalRootSignatureDesc& desc);

	void CreateStateObject(const RaytracingShaderBlobGroup* blobGroup);

	void SetStateObject();

	//* getter *//

	ID3D12StateObjectProperties* GetProperties() const { return properties_.Get(); }

	size_t GetMaxBufferSize(ExportGroupType type) const { return maxBufferSizes_[static_cast<uint32_t>(type)]; }

	uint32_t GetExportCount(ExportGroupType type) const { return exportCounts_[static_cast<uint32_t>(type)]; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	template <class T>
	using ExportTypeArray = std::array<T, static_cast<uint32_t>(ExportGroupType::kCountOfExportGroupType)>;

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* ID3D12 *//

	ComPtr<ID3D12RootSignature>         globalRootSignature_;
	ComPtr<ID3D12StateObject>           stateObject_;
	ComPtr<ID3D12StateObjectProperties> properties_;

	//* parameter *//

	ExportTypeArray<size_t>   maxBufferSizes_;
	ExportTypeArray<uint32_t> exportCounts_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDxilLibrary(CD3DX12_STATE_OBJECT_DESC& desc, const RaytracingShaderBlobGroup* blobGroup);
	void CreateGlobalRootSignature(CD3DX12_STATE_OBJECT_DESC& desc);
	void CreateLocalRootSignature(CD3DX12_STATE_OBJECT_DESC& desc, const RaytracingShaderBlobGroup* blobGroup);
	void CreateShaderConfig(CD3DX12_STATE_OBJECT_DESC& desc);
	void CreatePipelineConfig(CD3DX12_STATE_OBJECT_DESC& desc);

	void AddExportCount(ExportGroupType type);
	void ReplaceMaxBufferSize(ExportGroupType type, LocalRootSignature* rootSignature);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RayTracingPayload structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RayTracingPayload { //!< raytracing shaderと同じ構造体
		Color4f color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RayTracingAttribute structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RayTracingAttribute { //!< raytracing shaderと同じ構造体
		Vector2f attribute;
	};

	static const UINT kMaxRecursionDepth = 16;
	static_assert(
		kMaxRecursionDepth <= D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH,
		"[RayTracing Error]: _DXROBJECT StateObject\n [UINT kMaxRecursionDepth] over D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH" //!< 最大31回であることの保障
	);

};


_DXROBJECT_NAMESPACE_END