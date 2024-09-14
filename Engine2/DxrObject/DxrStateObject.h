#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DxrObject
#include <DxrRootSignature.h>
#include <DxrShaderBlob.h>
#include <DxrExportGroup.h>

//* c++
#include <unordered_set>
#include <array>

//* Geometry
#include <Vector2.h>
#include <Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
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

	StateObject() = default;

	void CreateRootSignature(const GlobalRootSignatureDesc& desc);
	
	void CreateStateObject(const ShaderBlob* blobs);

	//* Pipeline *//

	void SetPipeline(ID3D12GraphicsCommandList6* commandList);

	//* getter *//

	ID3D12StateObjectProperties* GetProperties() const { return properties_.Get(); }

	const size_t GetExportSize(ExportGroupType type) const { return exports_[type].size(); }

	const size_t GetMaxTotalBufferSize(ExportGroupType type) const { return maxTotalBufferSize_[type]; }

private:


	//=========================================================================================
	// private variables
	//=========================================================================================

	//* ID3D12 *//

	ComPtr<ID3D12RootSignature>         globalRootSignature_;
	ComPtr<ID3D12StateObject>           stateObject_;
	ComPtr<ID3D12StateObjectProperties> properties_;

	//* exports container *//

	std::array<std::unordered_set<const ExportGroup*>, kCountOfExportGroupType> exports_;
	std::array<size_t, kCountOfExportGroupType>                                 maxTotalBufferSize_;

private: //* config *//

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
		"[RayTracing Error]: _DXROBJECT StateObject\n [UINT kMaxRecursionDepth] over D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH"
	);
	//!< 最大31回であることの保障
};

_DXROBJECT_NAMESPACE_END