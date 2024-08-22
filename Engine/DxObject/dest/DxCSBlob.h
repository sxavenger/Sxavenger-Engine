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
#include <DxShaderManager.h>

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

		CSBlob() = default;

		~CSBlob() { Term(); }

		//! @brief 初期化処理
		void Init(const std::wstring& fileName);

		//! @brief 終了処理
		void Term();

		IDxcBlob* GetBlob() const { return csBlob_; }

		static void SetShaderManager(ShaderManager* manager) { manager_ = manager; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* static manager */
		static ShaderManager* manager_;

		IDxcBlob* csBlob_ = nullptr;

	};

}