#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* DirectX12
#include <dxcapi.h>
#include <d3d12shader.h>

//* c++
#include <array>
#include <filesystem>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class
///////////////////////////////////////////////////////////////////////////////////////////
//! @brief Shaderのコンパイルを行うクラス.
class ShaderCompiler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* compiler opiton *//

	ComPtr<IDxcBlob> Compile(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	ComPtr<ID3D12ShaderReflection> Reflection(IDxcBlob* blob);

	static ShaderCompiler* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compiler *//

	ComPtr<IDxcUtils>          utils_;
	ComPtr<IDxcCompiler3>      compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	static const std::array<LPCWSTR, static_cast<uint8_t>(CompileProfile::lib) + 1> profiles_;

};

_DXOBJECT_NAMESPACE_END
