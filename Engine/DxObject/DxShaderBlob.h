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
#include <string>
#include <cassert>

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
	// ShaderBlob class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderBlob {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		//! 
		//! @param[in] vsFilePath vsファイルパス
		//! @param[in] psFilePath psファイルパス
		ShaderBlob(const std::wstring& vsFilePath, const std::wstring& psFilePath) {
			Init(vsFilePath, psFilePath);
		}

		//! @brief デストラクタ
		~ShaderBlob() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] vsFilePath vsファイルパス
		//! @param[in] psFilePath psファイルパス
		void Init(const std::wstring& vsFilePath, const std::wstring& psFilePath);

		//! @brief 終了処理
		void Term();

		//! @brief shaderBlob_VSを取得
		//! 
		//! @return shaderBlob_VSを返却
		IDxcBlob* GetShaderBlob_VS() const { return shaderBlob_VS_.Get(); }

		//! @brief shaderBlob_PSを取得
		//! 
		//! @return shaderBlob_PSを返却
		IDxcBlob* GetShaderBlob_PS() const { return shaderBlob_PS_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<IDxcUtils>     dxcUtils_;
		ComPtr<IDxcCompiler3> dxcCompiler_;

		ComPtr<IDxcIncludeHandler> includeHandler_;

		ComPtr<IDxcBlob> shaderBlob_VS_;
		ComPtr<IDxcBlob> shaderBlob_PS_;

	};

}