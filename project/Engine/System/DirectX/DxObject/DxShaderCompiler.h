#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* lib
#include <Lib/Sxl/Expected.h>

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
//! @brief DXC(DirectX Shader Compiler)をラップし, HLSLのコンパイルとリフレクションを提供するクラス (シングルトン)
class ShaderCompiler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief DXCのユーティリティ/コンパイラを初期化する
	void Init();

	//! @brief 保持リソースを破棄する
	void Term();

	//* compiler option *//

	//! @brief ファイルからシェーダーをコンパイルする
	//! @param[in] filepath   シェーダーファイルのパス
	//! @param[in] profile    コンパイルプロファイル
	//! @param[in] entryPoint エントリポイント名
	//! @return コンパイル済みBlob
	ComPtr<IDxcBlob> Compile(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	//! @brief ソースコード文字列からシェーダーをコンパイルする
	//! @param[in] filepath   インクルード解決の基準パス
	//! @param[in] code       シェーダーのソースコード
	//! @param[in] profile    コンパイルプロファイル
	//! @param[in] entryPoint エントリポイント名
	//! @return コンパイル済みBlob
	ComPtr<IDxcBlob> Compile(
		const std::filesystem::path& filepath,
		const std::string& code,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	//! @brief Blobからシェーダーリフレクション情報を取得する
	//! @param[in] blob 対象のシェーダーBlob
	//! @return シェーダーリフレクション
	ComPtr<ID3D12ShaderReflection> Reflection(IDxcBlob* blob);

	//* setter *//

	//! @brief 使用するシェーダーモデルTierを設定する
	//! @param[in] model シェーダーモデル
	void SetShaderModelTire(D3D_SHADER_MODEL model);

	//! @brief インラインレイトレーシング対応の有無を設定する
	//! @param[in] isSupport 対応するか
	void SetSupportInlineRaytracing(bool isSupport) { isSupportInlineRaytracing_ = isSupport; }

	//* singleton *//

	//! @brief シングルトンインスタンスを取得する
	//! @return インスタンスへのポインタ
	static ShaderCompiler* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Argument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief DXCへ渡すコンパイル引数を組み立て, 文字列の寿命を管理する構造体
	struct Argument {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 生ポインタの引数を追加する
		void PushArgument(LPCWSTR argument);

		//! @brief 文字列の引数を追加する(寿命を内部で保持)
		void PushArgument(const std::wstring& argument);

		//! @brief 入力ファイルパスを引数に追加する
		void AddFilepath(const std::filesystem::path& filepath);

		//! @brief プロファイル(-T)を引数に追加する
		void AddProfile(CompileProfile profile, const std::wstring& tire);

		//! @brief エントリポイント(-E)を引数に追加する
		void AddEntryPoint(const std::wstring& entryPoint);

		//! @brief プリプロセッサ定義(-D)を引数に追加する
		void AddDefine(LPCWSTR name);

		//* getter *//

		//! @brief 引数配列の先頭ポインタを取得する
		LPCWSTR* GetData() { return arguments_.data(); }

		//! @brief 引数の個数を取得する
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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxc compiler *//

	ComPtr<IDxcUtils>          utils_;
	ComPtr<IDxcCompiler3>      compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	//* shader stage *//

	static const std::array<LPCWSTR, static_cast<uint8_t>(CompileProfile::Lib) + 1> stages_;

	//* compiler features *//

	std::wstring tire_ = L"6_6";

	bool isSupportInlineRaytracing_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::wstring GetProfile(CompileProfile profile) const;

	Sxl::Expected<ComPtr<IDxcBlob>, std::string> Compile(
		const std::filesystem::path& filepath,
		IDxcBlobEncoding* source,
		CompileProfile profile,
		std::wstring entryPoint = L""
	);

};

DXOBJECT_NAMESPACE_END
