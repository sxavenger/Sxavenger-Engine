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
#include <DxShaderTable.h>

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
		ShaderBlob() {}

		//! @brief デストラクタ
		~ShaderBlob() { Term(); }

		//! @brief shaderBlobの生成
		void Create(const std::wstring& fileName, ShaderType type);

		//! @brief 終了処理
		void Term();

		//! @brief shaderBlobを取得
		//! 
		//! @return shaderBlobを返却
		IDxcBlob* GetShaderBlob(ShaderType shaderType) const { return shaderBlob_[shaderType]; }

		//! @brief shaderTableを設定
		//! 
		//! @param[in] shaderTable DxObject::ShaderTable
		static void SetShaderTable(ShaderTable* shaderTable) { shaderTable_ = shaderTable; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static ShaderTable* shaderTable_;

		IDxcBlob* shaderBlob_[ShaderType::kCountOfShaderType];

	};

}