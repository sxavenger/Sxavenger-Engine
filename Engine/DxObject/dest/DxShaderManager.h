#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <d3d12shader.h>

// c++
#include <cstdint>
#include <string>
#include <unordered_map>

// ComPtr
#include <ComPtr.h>

// DxObject
#include <DxObjectMethod.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ShaderType {
		// rendering pipeline
		// vertex pipeline
		VERTEX,
		GEOMETRY,

		// mesh pipeline
		MESH,
		AMPLIFICATION,

		PIXEL,

		// compute pipeline
		COMPUTE,

		kCountOfShaderType
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// shaderManager class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderManager {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		ShaderManager() { Init(); }

		//! @brief デストラクタ
		~ShaderManager() { Term(); }

		//! @brief 初期化処理
		void Init();

		//! @brief 終了処理
		void Term();

		IDxcBlob* GetBlob(const std::wstring& filePath, ShaderType type);

		IDxcUtils* GetUtils() const { return dxcUtils_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* dxc compiler */

		ComPtr<IDxcUtils>          dxcUtils_;
		ComPtr<IDxcCompiler3>      dxcCompiler_;
		ComPtr<IDxcIncludeHandler> includeHandler_;

		static const LPCWSTR compileModel_[kCountOfShaderType];

		/* blob container */

		std::unordered_map<std::wstring, ComPtr<IDxcBlob>> blobs_; 

		/* parameter */

		static const std::wstring directoryPath_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void InitDxcCompiler();

		ComPtr<IDxcBlob> CreateBlob(const std::wstring& filePath, ShaderType type);

	};

}