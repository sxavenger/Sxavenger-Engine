#include "DxShaderManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Log
#include <Logger.h>

// DxObject static use
#include <DxGraphicsBlob.h>
#include <DxCSBlob.h>

//=========================================================================================
// static variables
//=========================================================================================

const LPCWSTR DxObject::ShaderManager::compileModel_[kCountOfShaderType] = {
	/* graphics pipeline */
	// vertex pipeline
	L"vs_6_5", //!< vs
	L"gs_6_5", //!< gs

	// mesh pipeline
	L"ms_6_5", //!< ms
	// AMP

	L"ps_6_5", //!< pixel

	/* compute pipeline */
	L"cs_6_5", //!< cs
};


const std::wstring DxObject::ShaderManager::directoryPath_ = L"./resources/hlsl/";

////////////////////////////////////////////////////////////////////////////////////////////
// shaderManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderManager::Init() {

	InitDxcCompiler();

	// managerのstatic設定
	GraphicsBlob::SetShaderManager(this);
	CSBlob::SetShaderManager(this);
}

void DxObject::ShaderManager::Term() {
	blobs_.clear();

	// managerのstatic設定の解除
	GraphicsBlob::SetShaderManager(nullptr);
	CSBlob::SetShaderManager(nullptr);
}

IDxcBlob* DxObject::ShaderManager::GetBlob(const std::wstring& filePath, ShaderType type) {
	// containerにblobがあるか確認
	if (blobs_.find(filePath) != blobs_.end()) { //!< containerにcompileされたblobがある場合
		return blobs_.at(filePath).Get();

	} else {
		//!< ないので新しくコンパイルして返却
		ComPtr<IDxcBlob> blob = CreateBlob(filePath, type);;
		blobs_[filePath]      = blob;
		return blob.Get();
	}
}


void DxObject::ShaderManager::InitDxcCompiler() {

	// dxcCompilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderManager]: dxcUtils_ << Complete Create");


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderManager]: dxcCompiler_ << Complete Create");
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderManager]: includeHandler_ << Complete Create");
	}

}

ComPtr<IDxcBlob> DxObject::ShaderManager::CreateBlob(const std::wstring& filePath, ShaderType type) {

	ComPtr<IDxcBlob> result;

	result = DxObjectMethod::CompileShader(
		directoryPath_ + filePath,
		compileModel_[type],
		dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get()
	);

	assert(result != nullptr);

	return result;
}

