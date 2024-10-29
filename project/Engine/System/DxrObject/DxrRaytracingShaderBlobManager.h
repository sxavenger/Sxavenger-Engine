#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>

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
// RaytracingShaderBlobManager class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingShaderBlobManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingShaderBlobManager()  { Init(); }
	~RaytracingShaderBlobManager() { Term(); }

	void Init();

	void Term();

	//* container option *//

	IDxcBlob* GetBlob(const std::wstring& filename);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compilers *//

	//* test *//

	ComPtr<IDxcUtils>          dxcUtils_;
	ComPtr<IDxcCompiler3>      dxcCompiler_;
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

	void CreateCompilers();

	void SetExternal(RaytracingShaderBlobManager* manager);

	ComPtr<IDxcBlob> CreateShaderBlob(const std::wstring& filepath);

	static std::wstring ToLower(const std::wstring& string);

};

_DXROBJECT_NAMESPACE_END
