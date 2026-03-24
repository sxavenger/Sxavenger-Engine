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
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderBlob() = default;

	void Create(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entry = L"main");

	//* operator [assign] *//

	ShaderBlob(const ComPtr<IDxcBlob>& blob) : blob_(blob) {}
	ShaderBlob& operator=(const ComPtr<IDxcBlob>& blob) { blob_ = blob; return *this; }

	ShaderBlob(ComPtr<IDxcBlob>&& blob) : blob_(std::move(blob)) {}
	ShaderBlob& operator=(ComPtr<IDxcBlob>&& blob) { blob_ = std::move(blob); return *this; }

	//* operator [comparison] *//

   bool operator==(std::nullptr_t) const { return blob_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return blob_ != nullptr; }

	//* getter *//

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

DXOBJECT_NAMESPACE_END
