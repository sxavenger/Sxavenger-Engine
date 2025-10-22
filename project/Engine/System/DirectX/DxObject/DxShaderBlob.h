#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxShaderCompiler.h"

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderBlob()  = default;
	~ShaderBlob() = default;

	void Create(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entrypoint = L"main");

	//* gettter *//

	IDxcBlob* GetBlob() const { return blob_.Get(); }

	D3D12_SHADER_BYTECODE GetBytecode() const;

	ComPtr<ID3D12ShaderReflection> GetReflection() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* blob *//

	ComPtr<IDxcBlob> blob_;

};

_DXOBJECT_NAMESPACE_END
