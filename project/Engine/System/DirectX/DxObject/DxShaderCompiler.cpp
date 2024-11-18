#include "DxShaderCompiler.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <vector>

//=========================================================================================
// static variables
//=========================================================================================

const std::array<LPCWSTR, static_cast<uint32_t>(CompileProfile::lib) + 1> ShaderCompiler::profiles_ = {
	L"vs_6_6", //!< vs
	L"gs_6_6", //!< gs
	L"ms_6_6", //!< ms
	L"as_6_6", //!< as
	L"ps_6_6", //!< ps
	L"cs_6_6", //!< cs
	L"lib_6_6" //!< lib
};

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Init() {

	// dxcCompilerの初期化
	auto hr = DxcCreateInstance(
		CLSID_DxcUtils, IID_PPV_ARGS(&utils_)
	);
	Assert(SUCCEEDED(hr));


	hr = DxcCreateInstance(
		CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)
	);
	Assert(SUCCEEDED(hr));

	// includeHandleの初期化
	hr = utils_->CreateDefaultIncludeHandler(&includeHandler_);
	Assert(SUCCEEDED(hr));

	EngineLog("[_DXOBJECT]::ShaderCompiler complete init.");
}

void ShaderCompiler::Term() {
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(
	const std::wstring& filepath,
	CompileProfile profile,
	const std::wstring& entryPoint) {

	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = utils_->LoadFile(filepath.c_str(), nullptr, &shaderSource);
	AssertW(SUCCEEDED(hr), L"hlsl not found. filepath: " + filepath);

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	// 基本情報の設定
	std::vector<LPCWSTR> arguments = {
		filepath.c_str(),                                 //!< コンパイル対象のhlslファイルパス
		L"-T", profiles_[static_cast<uint32_t>(profile)], //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",                         //!< デバッグ用情報を埋め込む
		L"-Od",                                           //!< 最適化を外しておく
		L"-Zpr",                                          //!< メモリレイアウトは行優先
	};

	//!< entry pointがある場合, 設定
	if (!entryPoint.empty()) {
		arguments.emplace_back(L"-E");
		arguments.emplace_back(entryPoint.c_str());
	}

	// compile
	ComPtr<IDxcResult> shaderResult;
	hr = compiler_->Compile(
		&shaderSourceBuffer,
		arguments.data(),
		static_cast<UINT32>(arguments.size()),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	Assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// hlslのerrorを出力
		Log("\n[hlsl compile error]\n " + std::string(shaderError->GetStringPointer()));
		AssertW(false, L"hlsl is compile error. filepath: " + filepath);
	}

	ComPtr<IDxcBlob> blob;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr);
	Assert(SUCCEEDED(hr));

	return blob;
}
