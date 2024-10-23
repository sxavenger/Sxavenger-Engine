#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cwctype>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlobManager class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBlobManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderBlobManager() { Init(); }

	~ShaderBlobManager() { Term(); }

	void Init();

	void Term();

	//* container option *//

	IDxcBlob* const GetBlob(const std::wstring& filename);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compilers *//

	ComPtr<IDxcLibrary>        library_;
	ComPtr<IDxcCompiler>       compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	//* container *//

	//! [unordered_map]
	//! key:   filename(すべて小文字) // directory + filenameでfilePath
	//! value: compileされたshaderBlob
	std::unordered_map<std::wstring, ComPtr<IDxcBlob>> blobs_;

	//* parameter *//

	static const LPCWSTR compilerModel_;
	static const std::wstring directoryPath_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitCompilers();
	void SetExternal(ShaderBlobManager* manager);

	ComPtr<IDxcBlob> CreateShaderBlob(const std::wstring& filepath);
	std::wstring ToLower(const std::wstring& string);

};

_DXROBJECT_NAMESPACE_END