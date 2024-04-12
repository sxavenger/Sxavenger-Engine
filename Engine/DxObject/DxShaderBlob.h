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
		ShaderBlob(const std::wstring& vsFileName, const std::wstring& psFileName) {
			Init(vsFileName, psFileName);
		}

		//! @brief コンストラクタ
		//! 
		//! @param[in] vsFileName directory_ + vsfileName
		//! @param[in] gsFileName directory_ + gsfileName
		//! @param[in] psFileName directory_ + psfileName
		ShaderBlob(const std::wstring& vsFileName, const std::wstring& gsFileName, const std::wstring& psFileName) {
			Init(vsFileName, gsFileName, psFileName);
		}

		//! @brief デストラクタ
		~ShaderBlob() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] vsFileName vsファイルパス
		//! @param[in] psFileName psファイルパス
		void Init(const std::wstring& vsFileName, const std::wstring& psFileName);

		//! @brief 初期化処理
		//! 
		//! @param[in] vsFileName vsファイルパス
		//! @param[in] gsFileName gsファイルパス
		//! @param[in] psFileName psファイルパス
		void Init(const std::wstring& vsFileName, const std::wstring& gsFileName, const std::wstring& psFileName);

		//! @brief 終了処理
		void Term();

		//! @brief shaderBlob_VSを取得
		//! 
		//! @return shaderBlob_VSを返却
		IDxcBlob* GetShaderBlob_VS() const { return shaderBlob_VS_; }

		//! @brief shaderBlob_PSを取得
		//! 
		//! @return shaderBlob_PSを返却
		IDxcBlob* GetShaderBlob_PS() const { return shaderBlob_PS_; }

		//! @brief shaderBlob_GSを取得
		//! 
		//! @return shaderBlob_GSを返却
		IDxcBlob* GetShaderBlob_GS() const { return shaderBlob_GS_; }

		//! @brief shaderTableを設定
		//! 
		//! @param[in] shaderTable DxObject::ShaderTable
		static void SetShaderTable(ShaderTable* shaderTable) { shaderTable_ = shaderTable; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static ShaderTable* shaderTable_;

		IDxcBlob* shaderBlob_VS_;
		IDxcBlob* shaderBlob_GS_;
		IDxcBlob* shaderBlob_PS_;

	};

}