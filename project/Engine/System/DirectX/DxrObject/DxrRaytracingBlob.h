#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxShaderCompiler.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <filesystem>

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

	//* getter *//

	D3D12_SHADER_BYTECODE GetBytecode() const;

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

};

_DXROBJECT_NAMESPACE_END
