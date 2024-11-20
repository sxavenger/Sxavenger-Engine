#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* DirectX12
#include <dxcapi.h>

//* c++
#include <array>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CompileProfile enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class CompileProfile : uint32_t {
	vs,
	gs,
	ms,
	as,
	ps,
	cs,
	lib,
};

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderCompiler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderCompiler()  = default;
	~ShaderCompiler() { Term(); }

	void Init();

	void Term();

	//* compiler opiton *//

	ComPtr<IDxcBlob> Compile(
		const std::wstring& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compiler *//

	ComPtr<IDxcUtils>          utils_;
	ComPtr<IDxcCompiler3>      compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	static const std::array<LPCWSTR, static_cast<uint32_t>(CompileProfile::lib) + 1> profiles_;

	static const std::wstring kDirectory_;

};

_DXOBJECT_NAMESPACE_END