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
#include <vector>

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
	// ShaderType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ShaderType {
		RAYGENERATION_SHAEAR,
		CLOSESTHIT_SHADER,
		MISS_SHADER,

		kCountOfShaderType
	};

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class ShaderManager;

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ShaderData {
		std::wstring mainFunctionName;
		ShaderType   shaderType;

		std::wstring hitgroup = L""; //!< closest, any... に必須
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderBlob class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderBlob {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~ShaderBlob() { Term(); }

		//! @brief stateObjectに送るためのshaderDataの設定
		//! 
		//! @param[in] filePath      shaderTable::directory_ からの相対パス
		//! @param[in] raygeneration raygenerationシェーダーのメイン関数名
		//! @param[in] closesthit    closesthitシェーダーのメイン関数名
		//! @param[in] miss          missシェーダーのメイン関数名
		//void Init(
		//	const std::wstring& filePath,
		//	const std::wstring& raygeneration, const std::wstring& closesthit, const std::wstring& miss //!< mainFunctionName
		//);

		void Create(const std::wstring& filePath);

		void SetShader(const std::wstring& mainFunctionName, ShaderType type, const std::wstring hitgroup = L"");

		void Term();

		IDxcBlob* GetShaderBlob() const { return blob_; }

		const std::vector<ShaderData>& GetDatas() const { return datas_; }

		static void SetShaderManager(ShaderManager* manager) { manager_ = manager; }

	private:

		


		//=========================================================================================
		// private variables
		//=========================================================================================

		static ShaderManager* manager_;

		// blob
		IDxcBlob* blob_;

		// datas_
		std::vector<ShaderData> datas_;
		//!< shaderに含まれるraygeneration等々のshader情報

	};

}