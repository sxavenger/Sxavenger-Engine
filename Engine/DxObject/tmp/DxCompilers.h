#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>

// c++
#include <cassert>

#include <ComPtr.h>

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
	class ShaderBlob;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Compilers class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Compilers {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		//! @brief コンストラクタ
		Compilers() { Init(); }

		//! @brief デストラクタ
		~Compilers() { Term(); }

		//! @brief 初期化処理
		void Init();

		//! @brief 終了処理
		void Term();

		//! @brief dxcUtilsの取得
		//! 
		//! @return IDxcUtilsの返却
		IDxcUtils* GetDxcUtils() const { return dxcUtils_.Get(); }

		//! @brief dxcCompilerの取得
		//! 
		//! @return dxcCompilerの返却
		IDxcCompiler3* GetDxcCompilder() const { return dxcCompiler_.Get(); }

		//! @brief includeHandlerの取得
		//! 
		//! @return includeHandlerの返却
		IDxcIncludeHandler* GetIncluderHandler() const { return includeHandler_.Get(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<IDxcUtils>     dxcUtils_;
		ComPtr<IDxcCompiler3> dxcCompiler_;

		ComPtr<IDxcIncludeHandler> includeHandler_;

	};

}