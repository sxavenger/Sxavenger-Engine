#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>

// c++
#include <cstdint>
#include <cassert>
#include <string>
#include <unordered_map>

// com
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderTable class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderManager {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		ShaderManager() { Init(); }

		~ShaderManager() { Term(); }

		void Init();

		void Term();

		IDxcBlob* GetShaderBlob(const std::wstring& filePath);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		// table

		std::unordered_map<std::wstring, ComPtr<IDxcBlob>> blobs_;

		// compilers

		ComPtr<IDxcLibrary>        library_;
		ComPtr<IDxcCompiler>       compiler_;
		ComPtr<IDxcIncludeHandler> includeHandler_;

		static const std::wstring shaderModel_;
		static const std::wstring directory_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath);

	};

}