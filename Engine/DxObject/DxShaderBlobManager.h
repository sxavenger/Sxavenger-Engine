#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <string>
#include <algorithm>
#include <cwctype>
#include <unordered_map>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderType {
	//* rendering pipeline *//
	//!< vertex pipeline
	VERTEX,
	GEOMETRY,

	//!< mesh pipeline
	MESH,
	AMPLIFICATION,

	//!< pixel shader
	PIXEL,

	//* compute pipeline *//
	COMPUTE,

	kCountOfShaderType
};

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

	//! @brief 初期化処理
	void Init();

	//! @brief 終了処理
	void Term();

	//* container option *//

	IDxcBlob* const GetBlob(const std::wstring& filePath, ShaderType type);

	//* getter *//

	IDxcUtils* const GetUtils() const { return dxcUtils_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DxcCompiler *//

	ComPtr<IDxcUtils>          dxcUtils_;
	ComPtr<IDxcCompiler3>      dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	//* blob container *//
	
	//! [unordered_map]
	//! key:   filePath(すべて小文字)
	//! value: blob
	std::unordered_map<std::wstring, ComPtr<IDxcBlob>> blobs_;

	//* Config *//

	static const LPCWSTR compilerModels_[ShaderType::kCountOfShaderType];

	static const std::wstring directoryPath_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitDxcCompiler();

	void SetExternal(ShaderBlobManager* manager);

	ComPtr<IDxcBlob> CreateShaderBlob(const std::wstring& filePath, ShaderType type);

};

_DXOBJECT_NAMESPACE_END