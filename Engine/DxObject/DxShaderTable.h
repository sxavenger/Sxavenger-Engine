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
#include <unordered_map>

// com
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
// ShaderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderType {
	MESH,
	PIXEL,

	kCountOfShaderType,
};

////////////////////////////////////////////////////////////////////////////////////////////
// namespace DxObject
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class Compilers;

	////////////////////////////////////////////////////////////////////////////////////////////
	// class ShaderTable
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderTable {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		ShaderTable() { Init(); }

		//! @brief デストラクタ
		~ShaderTable() { Term(); }

		//! @brief 初期化処理
		void Init();

		//! @brief 終了処理
		void Term();

		IDxcBlob* GetShaderBlob(const std::wstring& filePath, ShaderType type) {
			// mapにshaderDataがあるか確認
			if (shaderBlobs_.find(filePath) != shaderBlobs_.end()) {
				return shaderBlobs_.at(filePath).Get();

			} else {
				CreateShaderBlob(filePath, type);
				return shaderBlobs_.at(filePath).Get();
			}
		}

		//! @brief Compilersの設定
		//! 
		//! @param[in] compilers DxObject::Compilers
		static void SetCompilers(Compilers* compilers) {
			compilers_ = compilers;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static Compilers* compilers_;
		static const std::wstring directory_;

		std::unordered_map<std::wstring, ComPtr<IDxcBlob>> shaderBlobs_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateShaderBlob(const std::wstring& filePath, ShaderType type);

	};
}