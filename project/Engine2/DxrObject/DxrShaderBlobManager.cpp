#include "DxrShaderBlobManager.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* externals
#include <DxrShaderBlob.h>

//=========================================================================================
// static variables
//=========================================================================================

const LPCWSTR ShaderBlobManager::compilerModel_      = L"lib_6_3";
const std::wstring ShaderBlobManager::directoryPath_ = L"./resources/raytracingShader/";

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlobManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBlobManager::Init() {
	InitCompilers();
	SetExternal(this);
}

void ShaderBlobManager::Term() {
	SetExternal(nullptr);
}

IDxcBlob* const ShaderBlobManager::GetBlob(const std::wstring& filename) {
	
	// 小文字に変換
	std::wstring lowerFilename = ToLower(filename);

	// containerにblobが存在しているか確認
	auto it = blobs_.find(lowerFilename);
	if (it == blobs_.end()) {//!< containerにcompileされたblobがない場合
		//!< ないので新しくコンパイルして返却
		ComPtr<IDxcBlob> blob = CreateShaderBlob(directoryPath_ + lowerFilename);
		blobs_.insert({ lowerFilename, blob });
	}

	return blobs_.at(lowerFilename).Get();
}

void ShaderBlobManager::InitCompilers() {

	auto hr = DxcCreateInstance(
		CLSID_DxcLibrary, IID_PPV_ARGS(&library_)
	);
	Assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(
		CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)
	);
	Assert(SUCCEEDED(hr));

	hr = library_->CreateIncludeHandler(&includeHandler_);
	Assert(SUCCEEDED(hr));
}

void ShaderBlobManager::SetExternal(ShaderBlobManager* manager) {
	ShaderBlob::SetShaderBlobManager(manager);
}

ComPtr<IDxcBlob> ShaderBlobManager::CreateShaderBlob(const std::wstring& filepath) {
	
	ComPtr<IDxcBlobEncoding> source;

	auto hr = library_->CreateBlobFromFile(
		filepath.c_str(), nullptr, &source
	);
	
	if (FAILED(hr)) {
		std::wstring outputLog = L"RayTracing Error : HLSL Not Found. filePath: ";
		AssertW(false, L"hlsl not found. filepath: " + filepath);
	}

	LPCWSTR arguments[] = { // test
		filepath.c_str(),         //!< コンパイル対象のhlslファイルパス
		L"-T", compilerModel_,    //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", //!< デバッグ用情報を埋め込む
		L"-Od",                   //!< 最適化を外しておく
	};

	ComPtr<IDxcOperationResult> compileResult;

	hr = compiler_->Compile(
		source.Get(),
		filepath.c_str(),
		L"",
		compilerModel_,
		arguments,
		_countof(arguments),
		nullptr,
		0,
		includeHandler_.Get(),
		&compileResult
	);

	// エラーの出力
	ComPtr<IDxcBlobEncoding> errorBlob;

	hr = compileResult->GetErrorBuffer(
		&errorBlob
	);

	if (SUCCEEDED(hr)) {
		if (errorBlob != nullptr && errorBlob->GetBufferSize() != 0) {
			// エラーメッセージの出力
			Assert(false, reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
		}
	}

	ComPtr<IDxcBlob> result;

	hr = compileResult->GetResult(
		&result
	);

	std::wstring log = L"[_DXROBJECT ShaderManager]: " + filepath + L" << Complete Compile";
	Log(log);
	
	return result;
}

std::wstring ShaderBlobManager::ToLower(const std::wstring& string) {

	std::wstring result;
	result.resize(string.size());

	std::transform(string.begin(), string.end(), result.begin(),
		[](wchar_t c) { return std::towlower(c); });

	return result;
}
