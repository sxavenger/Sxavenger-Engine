#include "DxShaderCompiler.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* external
#include <magic_enum.hpp>

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
	Exception::Assert(SUCCEEDED(hr));


	hr = DxcCreateInstance(
		CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)
	);
	Exception::Assert(SUCCEEDED(hr));

	// includeHandleの初期化
	hr = utils_->CreateDefaultIncludeHandler(&includeHandler_);
	Exception::Assert(SUCCEEDED(hr));

	Logger::EngineLog("[_DXOBJECT ShaderCompiler] complete initialize.");
}

void ShaderCompiler::Term() {
	Logger::EngineLog("[_DXOBJECT ShaderCompiler] term.");
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	CompileProfile profile,
	const std::wstring& entryPoint) {

	std::wstring filepathW = filepath.generic_wstring(); //!< wstringの寿命確保

	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = utils_->LoadFile(filepathW.c_str(), nullptr, &shaderSource);
	Exception::Assert(SUCCEEDED(hr), "hlsl not found.", "filepath: " + filepath.generic_string());

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	// 基本情報の設定
	std::vector<LPCWSTR> arguments = {
		filepathW.c_str(),                                //!< コンパイル対象のhlslファイルパス
		L"-T", profiles_[static_cast<uint32_t>(profile)], //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",                         //!< デバッグ用情報を埋め込む
		L"-Zpr",                                          //!< メモリレイアウトは行優先
	};

#ifdef _DEVELOPMENT
	arguments.emplace_back(
		SxavengerConfig::GetConfig().isEnableShaderOptimization ? L"-O3" : L"-Od"
	);
#else
	// releaseではoption関わらず最適化
	arguments.emplace_back(L"-O3"); //!< 最適化を最大にする
#endif

	if (!entryPoint.empty()) { //!< entry pointがある場合, 設定
		arguments.emplace_back(L"-E");
		arguments.emplace_back(entryPoint.c_str());
	}

	if (profile == CompileProfile::cs || profile == CompileProfile::lib) { //!< compute shader用のdefineを追加
		arguments.emplace_back(L"-D");
		arguments.emplace_back(L"_COMPUTE");
	}

	if (SxavengerConfig::GetSupport().isSupportInlineRaytracing) {
		arguments.emplace_back(L"-D");
		arguments.emplace_back(L"_INLINE_RAYTRACING");
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
	Exception::Assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Exception::Assert(false, "hlsl is compile error. filepath: " + filepath.generic_string(), shaderError->GetStringPointer());
	}

	ComPtr<IDxcBlob> blob;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr);
	Exception::Assert(SUCCEEDED(hr));

	Logger::EngineThreadLog(std::format("[_DXOBJECT ShaderCompiler] shader compiled. filepath: {}, profile: {}", filepath.generic_string(), magic_enum::enum_name(profile)));
	return blob;
}

ComPtr<ID3D12ShaderReflection> ShaderCompiler::Reflection(IDxcBlob* blob) {

	ComPtr<ID3D12ShaderReflection> result;

	DxcBuffer buffer = {};
	buffer.Size = blob->GetBufferSize();
	buffer.Ptr  = blob->GetBufferPointer();

	auto hr = utils_->CreateReflection(
		&buffer,
		IID_PPV_ARGS(&result)
	);
	Exception::Assert(SUCCEEDED(hr), "shader reflection is failed.");

	return result;
}
