#include "DxrRaytracingShaderBlobManager.h"
_DXROBJECT_USING;

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* externals
#include <Engine/System/DxrObject/DxrRaytracingShaderBlob.h>

//=========================================================================================
// static variables
//=========================================================================================

const LPCWSTR RaytracingShaderBlobManager::compilerModel_      = L"lib_6_4";
const std::wstring RaytracingShaderBlobManager::directoryPath_ = L"./resources/hlsl/raytracing/";

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingShaderBlobManager class
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingShaderBlobManager::Init() {
	CreateCompilers();
	SetExternal(this);
}

void RaytracingShaderBlobManager::Term() {
	SetExternal(nullptr);
}

IDxcBlob* RaytracingShaderBlobManager::GetBlob(const std::wstring& filename) {

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

void RaytracingShaderBlobManager::CreateCompilers() {

	// dxcCompilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_)
		);
		Assert(SUCCEEDED(hr));


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);
		Assert(SUCCEEDED(hr));
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
		Assert(SUCCEEDED(hr));
	}

}

void RaytracingShaderBlobManager::SetExternal(RaytracingShaderBlobManager* manager) {
	RaytracingShaderBlob::SetRaytracingShaderBlobManager(manager);
}

ComPtr<IDxcBlob> RaytracingShaderBlobManager::CreateShaderBlob(const std::wstring& filepath) {

	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = dxcUtils_->LoadFile(filepath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	if (FAILED(hr)) {
		AssertW(false, L"hlsl not found. filepath: " + filepath);
	}

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filepath.c_str(),             //!< コンパイル対象のhlslファイルパス
		L"-T", compilerModel_, //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",     //!< デバッグ用情報を埋め込む
		L"-Od",                       //!< 最適化を外しておく
		L"-Zpr",                      //!< メモリレイアウトは行優先
	};

	// ShaderCompile
	ComPtr<IDxcResult> shaderResult;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);

	Assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// hlslのerrorを出力
		Assert(false, shaderError->GetStringPointer());
	}

	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	Assert(SUCCEEDED(hr));

	return shaderBlob;
}

std::wstring RaytracingShaderBlobManager::ToLower(const std::wstring& string) {

	std::wstring result;
	result.resize(string.size());

	std::transform(string.begin(), string.end(), result.begin(),
		[](wchar_t c) { return std::towlower(c); });

	return result;
}
