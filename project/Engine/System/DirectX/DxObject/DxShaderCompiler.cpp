#include "DxShaderCompiler.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Configuration/Configuration.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <vector>

//=========================================================================================
// static variables
//=========================================================================================

const std::array<LPCWSTR, static_cast<uint32_t>(CompileProfile::Lib) + 1> ShaderCompiler::stages_ = {
	L"vs", //!< vertex shader
	L"gs", //!< geometry shader
	L"ms", //!< mesh shader
	L"as", //!< amplification shader
	L"ps", //!< pixel shader
	L"cs", //!< compute shader
	L"lib" //!< library (raytracing shader)
};

////////////////////////////////////////////////////////////////////////////////////////////
// Argument structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Argument::PushArgument(LPCWSTR argument) {
	arguments_.emplace_back(argument); //!< 引数を追加
}

void ShaderCompiler::Argument::PushArgument(const std::wstring& argument) {
	lifetime_.emplace_back(argument); //!< 引数の寿命を管理するためにリストに追加
	arguments_.emplace_back(lifetime_.back().c_str());
}

void ShaderCompiler::Argument::AddFilepath(const std::filesystem::path& filepath) {
	PushArgument(filepath.generic_wstring()); //!< ファイルパスの引数を追加
}

void ShaderCompiler::Argument::AddProfile(CompileProfile profile, const std::wstring& tire) {
	PushArgument(L"-T"); //!< プロフィールの引数を追加
	PushArgument(GetProfile(profile, tire)); //!< プロフィールの引数を追加
}

void ShaderCompiler::Argument::AddEntryPoint(const std::wstring& entryPoint) {
	PushArgument(L"-E");              //!< entry pointの引数を追加
	PushArgument(entryPoint.c_str());
}

void ShaderCompiler::Argument::AddDefine(LPCWSTR name) {
	PushArgument(L"-D"); //!< defineの引数を追加
	PushArgument(name);
}

std::wstring ShaderCompiler::Argument::GetProfile(CompileProfile profile, const std::wstring& tire) const {
	return std::format(L"{}_{}", stages_[static_cast<uint32_t>(profile)], tire);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Init() {

	// dxcCompilerの初期化
	auto hr = DxcCreateInstance(
		CLSID_DxcUtils, IID_PPV_ARGS(&utils_)
	);
	DxObject::Assert(hr, L"dxc utils create failed.");


	hr = DxcCreateInstance(
		CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)
	);
	DxObject::Assert(hr, L"dxc compiler create failed.");

	// includeHandleの初期化
	hr = utils_->CreateDefaultIncludeHandler(&includeHandler_);
	DxObject::Assert(hr, L"dxc include handler create failed.");

	StreamLogger::EngineLog("[DXOBJECT ShaderCompiler] complete initialize.");
}

void ShaderCompiler::Term() {
	utils_.Reset();
	compiler_.Reset();
	includeHandler_.Reset();
	StreamLogger::EngineLog("[DXOBJECT ShaderCompiler] term.");
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	CompileProfile profile,
	const std::wstring& entryPoint) {

	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> source;
	auto hr = utils_->LoadFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		&source
	);
	StreamLogger::AssertA(SUCCEEDED(hr), "hlsl not found.", "filepath: " + filepath.generic_string());

	auto blob = ShaderCompiler::Compile(
		filepath,
		source.Get(),
		profile,
		entryPoint
	);

	if (!blob.HasValue()) {
		StreamLogger::Exception("hlsl is compile error. filepath: " + filepath.generic_string(), blob.Error());
	}

	StreamLogger::EngineLog(std::format("[DXOBJECT ShaderCompiler] shader compiled. filepath: {}, profile: {}", filepath.generic_string(), magic_enum::enum_name(profile)));
	return blob.Value();
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	const std::string& code,
	CompileProfile profile,
	const std::wstring& entryPoint) {

	// コードを読み取る
	ComPtr<IDxcBlobEncoding> source;
	auto hr = utils_->CreateBlob(
		code.data(),
		static_cast<UINT32>(code.size()),
		DXC_CP_UTF8,
		&source
	);
	StreamLogger::AssertA(SUCCEEDED(hr), "source blob create failed.", "filepath: " + filepath.generic_string());

	auto blob = ShaderCompiler::Compile(
		filepath,
		source.Get(),
		profile,
		entryPoint
	);

	if (!blob.HasValue()) {
		StreamLogger::Exception("source code is compile error. filepath: " + filepath.generic_string(), blob.Error());
	}

	StreamLogger::EngineLog(std::format("[DXOBJECT ShaderCompiler] source code compiled. filepath: {}, profile: {}", filepath.generic_string(), magic_enum::enum_name(profile)));
	return blob.Value();
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
	DxObject::Assert(hr, L"shader reflection is failed.");

	return result;
}

void ShaderCompiler::SetShaderModelTire(D3D_SHADER_MODEL model) {
	uint32_t major = (model >> 4) & 0xF; //!< 上位4bitがmajor
	uint32_t minor = model & 0xF;        //!< 下位4bitがminor

	tire_ = std::format(L"{}_{}", major, minor);
}

ShaderCompiler* ShaderCompiler::GetInstance() {
	static ShaderCompiler instance;
	return &instance;
}

std::wstring ShaderCompiler::GetProfile(CompileProfile profile) const {
	return std::format(L"{}_{}", stages_[static_cast<uint32_t>(profile)], tire_);
}

Sxl::Expected<ComPtr<IDxcBlob>, std::string> ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	IDxcBlobEncoding* source,
	CompileProfile profile,
	std::wstring entryPoint) {

	// sourceの内容を設定する
	DxcBuffer buffer = {};
	buffer.Ptr      = source->GetBufferPointer();
	buffer.Size     = source->GetBufferSize();
	buffer.Encoding = DXC_CP_UTF8;

	Argument arguments;
	arguments.AddFilepath(filepath);      //!< コンパイル対象のファイルパス
	arguments.AddProfile(profile, tire_); //!< プロフィールの引数を追加
	arguments.PushArgument(L"-Zpr");      //!< メモリレイアウトは行優先

#ifdef _DEVELOPMENT
	//!< 最適化の有無を設定
	arguments.PushArgument(
		Configuration::GetConfig().enableShaderOptimization ? L"-O3" : L"-Od" //!< 最適化の有無
	);

	arguments.PushArgument(L"-Zi");           //!< デバッグ情報を生成
	arguments.PushArgument(L"-Qembed_debug"); //!< デバッグ情報をシェーダバイナリに埋め込む
#else
	//!< option関わらず最適化
	arguments.PushArgument(L"-O3"); //!< 最適化を最大にする
#endif

	if (!entryPoint.empty()) {
		arguments.AddEntryPoint(entryPoint); //!< entry pointの引数を追加
	}

	if (profile == CompileProfile::Compute || profile == CompileProfile::Lib) {
		arguments.AddDefine(L"_COMPUTE_SHADER"); //!< compute shader or library shaderであることをdefineで伝える
	}

	if (isSupportInlineRaytracing_) {
		arguments.AddDefine(L"_SUPPORT_INLINE_RAYTRACING"); //!< inline raytracingをサポートしていることをdefineで伝える
	}

	// shaderのコンパイル
	ComPtr<IDxcResult> result;
	auto hr = compiler_->Compile(
		&buffer,
		arguments.GetData(),
		arguments.GetCount(),
		includeHandler_.Get(),
		IID_PPV_ARGS(&result)
	);
	DxObject::Assert(hr, L"shader compile failed.");

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> error;
	hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr);

	if (error != nullptr && error->GetStringLength() != 0) {
		return Sxl::Unexpected<std::string>(error->GetStringPointer());
	}

	ComPtr<IDxcBlob> blob;
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr);
	DxObject::Assert(hr, L"shader compile output failed.");

	return blob;

}
