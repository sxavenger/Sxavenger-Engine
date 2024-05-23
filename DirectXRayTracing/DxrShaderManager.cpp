#include "DxrShaderManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

#include <DxrShaderBlob.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::wstring DxrObject::ShaderManager::shaderModel_ = L"lib_6_3";
const std::wstring DxrObject::ShaderManager::directory_ = L"./resources/raytracingShader/";

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::ShaderManager::Init() {

	// compilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcLibrary, IID_PPV_ARGS(&library_)
		);
		assert(SUCCEEDED(hr));

		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)
		);
		assert(SUCCEEDED(hr));
	}

	// tableをblobに設定
	DxrObject::ShaderBlob::SetShaderManager(this);

}

void DxrObject::ShaderManager::Term() {
	blobs_.clear();
	DxrObject::ShaderBlob::SetShaderManager(nullptr);
}

IDxcBlob* DxrObject::ShaderManager::GetShaderBlob(const std::wstring& filePath) {
	// mapにshaderDataがあるか確認
	if (blobs_.find(filePath) != blobs_.end()) { //!< すでにコンパイルしたBlobがある場合
		return blobs_.at(filePath).Get();

	} else {
		// blobの登録
		blobs_[filePath] = CompileShader(directory_ + filePath);
		return blobs_.at(filePath).Get();
	}

}

//=========================================================================================
// private methods
//=========================================================================================

ComPtr<IDxcBlob> DxrObject::ShaderManager::CompileShader(const std::wstring& filePath) {

	ComPtr<IDxcBlobEncoding> source;

	auto hr = library_->CreateBlobFromFile(
		filePath.c_str(), nullptr, &source
	);
	assert(SUCCEEDED(hr)); //!< filePath error

	// コンパイル
	ComPtr<IDxcOperationResult> compileResult;

	hr = compiler_->Compile(
		source.Get(),
		filePath.c_str(),
		L"",
		shaderModel_.c_str(), //!< shaderModel
		nullptr, 0, nullptr, 0, nullptr,
		&compileResult
	);
	assert(SUCCEEDED(hr));

	// エラーの出力
	ComPtr<IDxcBlobEncoding> errorBlob;

	hr = compileResult->GetErrorBuffer(
		&errorBlob
	);

	if (SUCCEEDED(hr)) {
		if (errorBlob != nullptr && errorBlob->GetBufferSize() != 0) {
			// エラーメッセージの出力
			Log("//-----------------------------------------------------------------------------------------");
			Log(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			Log("//-----------------------------------------------------------------------------------------");
			assert(false);
		}
	}

	ComPtr<IDxcBlob> result;

	hr = compileResult->GetResult(
		&result
	);

	std::wstring logstr = L"[DxrObject::ShaderManager]: " + filePath + L" << Complete Create";
	Log(logstr);

	return result;

}