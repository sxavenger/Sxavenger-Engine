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

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class ShaderTable;

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
		//! @param[in] vsFileName directory_ + vsfileName
		//! @param[in] psFileName directory_ + psfileName
		ShaderBlob(const std::wstring& msFileName, const std::wstring& psFileName) {
			Init(msFileName, psFileName);
		}

		//! @brief デストラクタ
		~ShaderBlob() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] vsFileName vsファイルパス
		//! @param[in] psFileName psファイルパス
		void Init(const std::wstring& msFileName, const std::wstring& psFileName);

		//! @brief 終了処理
		void Term();

		//! @brief shaderBlob_MSを取得
		//! 
		//! @return shaderBlob_MSを返却
		IDxcBlob* GetShaderBlob_MS() const { return shaderBlob_MS_; }

		//! @brief shaderBlob_PSを取得
		//! 
		//! @return shaderBlob_PSを返却
		IDxcBlob* GetShaderBlob_PS() const { return shaderBlob_PS_; }

		//! @brief shaderTableを設定
		//! 
		//! @param[in] shaderTable DxObject::ShaderTable
		static void SetShaderTable(ShaderTable* shaderTable) { shaderTable_ = shaderTable; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static ShaderTable* shaderTable_;

		IDxcBlob* shaderBlob_MS_;
		IDxcBlob* shaderBlob_PS_;

	};

}