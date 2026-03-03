#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* DirectX12
#include <dxcapi.h>
#include <d3d12shader.h>

//* c++
#include <array>
#include <filesystem>
#include <list>
#include <string>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderCompiler {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Argument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Argument {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void PushArgument(LPCWSTR argument);

		void PushArgument(const std::wstring& argument);

		void AddFilepath(const std::filesystem::path& filepath);

		void AddProfile(CompileProfile profile, const std::wstring& tire);

		void AddEntryPoint(const std::wstring& entryPoint);

		void AddDefine(LPCWSTR name);

		//* getter *//

		LPCWSTR* GetData() { return arguments_.data(); }

		UINT32 GetCount() const { return static_cast<UINT32>(arguments_.size()); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::vector<LPCWSTR> arguments_; //!< コンパイルオプションの引数

		std::list<std::wstring> lifetime_; //!< 引数の寿命を管理するリスト

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::wstring GetProfile(CompileProfile profile, const std::wstring& tire) const;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* compiler option *//

	ComPtr<IDxcBlob> Compile(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	ComPtr<ID3D12ShaderReflection> Reflection(IDxcBlob* blob);

	//* setter *//

	void SetShaderModelTire(D3D_SHADER_MODEL model);

	void SetSupportInlineRaytracing(bool isSupport) { isSupportInlineRaytracing_ = isSupport; }

	//* singleton *//

	static ShaderCompiler* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compiler *//

	ComPtr<IDxcUtils>          utils_;
	ComPtr<IDxcCompiler3>      compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	//* shader stage *//

	static const std::array<LPCWSTR, static_cast<uint8_t>(CompileProfile::lib) + 1> stages_;

	//* compiler features *//

	std::wstring tire_ = L"6_6";

	bool isSupportInlineRaytracing_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::wstring GetProfile(CompileProfile profile) const;

};

DXOBJECT_NAMESPACE_END
