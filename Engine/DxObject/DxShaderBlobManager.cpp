#include "DxShaderBlobManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject static used
#include <DxShaderBlob.h>
#include <DxShaderReflection.h>

//=========================================================================================
// static variables
//=========================================================================================

const LPCWSTR DxObject::ShaderBlobManager::compilerModels_[ShaderType::kCountOfShaderType] = {
	//* graphics pipeline *//
	//!< vertex pipeline
	L"vs_6_5", //!< vs
	L"gs_6_5", //!< gs

	//!< mesh pipeline
	L"ms_6_5", //!< ms
	L"as_6_5", //!< as

	L"ps_6_5", //!< pixel

	//* compute pipeline *//
	L"cs_6_5", //!< cs
};


const std::wstring DxObject::ShaderBlobManager::directoryPath_ = L"./resources/hlsl/";

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlobManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderBlobManager::Init() {

	InitDxcCompiler();

	//* staticの設定 *//
	SetExternal(this);
}

void DxObject::ShaderBlobManager::Term() {
	SetExternal(nullptr);
}

IDxcBlob* const DxObject::ShaderBlobManager::GetBlob(const std::wstring& filePath, ShaderType type) {

	// 一度すべて小文字に変換
	std::wstring lowerFilePath = ToLower(filePath);

	auto it = blobs_.find(lowerFilePath);

	// containerにblobがあるか確認
	if (blobs_.find(lowerFilePath) == blobs_.end()) { //!< containerにcompileされたblobがない場合
		//!< ないので新しくコンパイルして返却
		ComPtr<IDxcBlob> blob = CreateShaderBlob(directoryPath_ + lowerFilePath, type);
		blobs_.insert({ lowerFilePath, blob });
	}

	return blobs_.at(lowerFilePath).Get();
}

void DxObject::ShaderBlobManager::InitDxcCompiler() {

	// dxcCompilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderBlobManager]: dxcUtils_ << Complete Create");


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderBlobManager]: dxcCompiler_ << Complete Create");
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

		assert(SUCCEEDED(hr));
		Log("[DxObject::ShaderBlobManager]: includeHandler_ << Complete Create");
	}

}

void DxObject::ShaderBlobManager::SetExternal(ShaderBlobManager* manager) {
	BaseShaderBlob::SetShaderBlobManager(manager);
}

ComPtr<IDxcBlob> DxObject::ShaderBlobManager::CreateShaderBlob(const std::wstring& filePath, ShaderType type) {
	
	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	if (FAILED(hr)) {
		std::wstring outputLog = L"Error : HLSL Not Found. filePath: ";
		Log(outputLog + filePath);

		std::string	errorLog;
		errorLog
			= "[HLSL Not Found] \n filePath: " + ToString(filePath);

		AssertMesseage(false, errorLog, "Error: CompileShader");
	}

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),             //!< コンパイル対象のhlslファイルパス
		L"-E", L"main",               //!< エントリーポイントの指定
		L"-T", compilerModels_[type], //!< ShaderProfileの設定
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

	assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {

		Log("//-----------------------------------------------------------------------------------------");
		Log(shaderError->GetStringPointer()); //!< shaderの構文error
		Log("//-----------------------------------------------------------------------------------------");

		// hlslのerrorをwindowで出力
		std::string	errorLog;
		errorLog
			= "[HLSL Error] \n filePath: " + ToString(filePath);

		AssertMesseage(false, errorLog, "Error: CompileShader");
	}

	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	return shaderBlob;
	
}

std::wstring DxObject::ShaderBlobManager::ToLower(const std::wstring& string) {

	std::wstring result;
	result.resize(string.size());

	std::transform(string.begin(), string.end(), result.begin(),
		[](wchar_t c) { return std::towlower(c); });

	return result;

}