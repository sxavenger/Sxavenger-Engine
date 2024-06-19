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
namespace DxObject { //!< DxSource
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// CSBlob class
	////////////////////////////////////////////////////////////////////////////////////////////
	class CSBlob {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		CSBlob() {}

		~CSBlob() { Term(); }

		//! @brief 初期化処理
		void Init(const std::wstring& fileName);

		//! @brief 終了処理
		void Term();

		//! @brief shaderTableの設定
		static void SetShaderTable(ShaderTable* shaderTable) { shaderTable_ = shaderTable; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static ShaderTable* shaderTable_;

		IDxcBlob* csBlob_ = nullptr;

	};

}