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
		RAYGENERATION_SHADER,
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