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
	Mesh,
	Pixel,

	kCountOfShaderType,
};

////////////////////////////////////////////////////////////////////////////////////////////
// namespace DxObject
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// RootDescs structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RootDescElement {
		std::vector<D3D12_ROOT_PARAMETER>      param;
		std::vector<D3D12_STATIC_SAMPLER_DESC> sampler;

		//! @brief 要素の削除
		void Clear() {
			param.clear();
			sampler.clear();
		}

		//! @brief 要素の統合
		void Intergrate(RootDescElement& other) {
			param.insert(param.end(), other.param.begin(), other.param.end());
			sampler.insert(sampler.end(), other.sampler.begin(), other.sampler.end());

			// 統合したのでotherのclear
			other.Clear();
		}

	};

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// class ShaderTable_test
	////////////////////////////////////////////////////////////////////////////////////////////
	//! rootDescsの自動化をするためのクラス
	//! 試作段階
	class ShaderTable_test {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		 //! @brief コンストラクタ
		ShaderTable_test() { Init(); }

		//! @brief デストラクタ
		~ShaderTable_test() { Term(); }

		void Init();

		void Term();

		IDxcBlob* GetShaderBlob(const std::wstring& filePath, ShaderType type) {
			// mapにshaderDataがあるか確認
			if (shaderDatas_.find(filePath) != shaderDatas_.end()) {
				return shaderDatas_.at(filePath).blob.Get();

			} else {
				CreateShaderElement(filePath, type);
				return shaderDatas_.at(filePath).blob.Get();
			}
		}

		//! @brief Compilersの設定
		//! 
		//! @param[in] compilers DxObject::Compilers
		static void SetCompilers(Compilers* compilers) {
			compilers_ = compilers;
		}

	private:

		////////////////////////////////////////////////////////////////////////////////////////////
		// ShaderData structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct ShaderElement {
			ComPtr<IDxcBlob> blob;
			RootDescElement  descs;
		};

		//=========================================================================================
		// private variables
		//=========================================================================================

		static Compilers* compilers_;
		static const std::wstring directory_;

		std::unordered_map<std::wstring, ShaderElement> shaderDatas_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void CreateShaderElement(const std::wstring& key, ShaderType type);

		RootDescElement GetRootDescElementOfShader(const std::wstring& filePath, ShaderType type);

		RootDescElement GetRootDescElementOfShader(const std::wstring& filePath, D3D12_SHADER_VISIBILITY visibility);

	};

}