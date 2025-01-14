#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrExportGroup.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxShaderCompiler.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <filesystem>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingBlob()  = default;
	~RaytracingBlob() = default;

	void Create(const std::filesystem::path& filepath);

	void SetExport(const ExportGroup* expt) { exports_.emplace(expt); }

	//* getter *//

	D3D12_SHADER_BYTECODE GetBytecode() const;

	std::unordered_set<const ExportGroup*> GetExports() const { return exports_; }

	//* setter *//

	static void SetExternal(DxObject::ShaderCompiler* compiler) { compiler_ = compiler; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	static DxObject::ShaderCompiler* compiler_;

	//* blob *//
	 
	ComPtr<IDxcBlob> blob_;

	//* export *//

	std::unordered_set<const ExportGroup*> exports_;

};

_DXROBJECT_NAMESPACE_END
