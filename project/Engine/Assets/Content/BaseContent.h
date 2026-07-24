#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/Runtime/Async/AsyncExecutionTask.h>
#include <Engine/Editors/Editor/InspectorEditor.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <filesystem>
#include <any>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseContent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ファイルから非同期に読み込まれるContent(モデル/テクスチャ等)の基底クラス
class BaseContent
	: public BaseInspector {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Contentの読み込み状態
	enum class Status : uint8_t {
		None,    //!< 初期状態
		Complete //!< 読み込み完了
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseContent(Async::Execution execution) : execution_(execution) {}

	virtual ~BaseContent() = default;

	//* content option *//

	//! @brief 読み込み対象のファイルパスとパラメータを関連付ける
	//! @param[in] filepath  対象ファイルのパス
	//! @param[in] parameter 読み込みパラメータ
	virtual void Attach(const std::filesystem::path& filepath, const std::any& parameter);

	//! @brief 実際の読み込み処理 (派生クラスで実装)
	//! @param[in] context DirectXのキューコンテキスト
	virtual void Load(MAYBE_UNUSED const DirectXQueueContext* context) = 0;

	//! @brief インスペクタ表示 (既定では何もしない)
	void ShowInspector() override {}

	//* parameter option *//

	//! @brief 関連付けられたファイルパスを取得する
	//! @return ファイルパス
	const std::filesystem::path& GetFilepath() const { return filepath_; }

	//! @brief 読み込みパラメータを取得する
	//! @return パラメータ
	const std::any& GetParameter() const { return parameter_; }

	//! @brief メタファイルの拡張子を取得する
	//! @return メタファイルの拡張子
	static const std::filesystem::path& GetMetaExtension() { return kMetaExtension; }

	//* execution option *//

	//! @brief このContentを処理する実行キュー種別を取得する
	//! @return 実行種別
	const Async::Execution GetExecution() const { return execution_; }

	//! @brief 読み込み完了まで呼び出しスレッドをブロックする
	void WaitComplete() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::filesystem::path filepath_;
	std::any parameter_;
	// HACK: Anyの廃止

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* meta helper methods *//

	static json LoadMetaData(const std::filesystem::path& filepath);

	static void SaveMetaData(const json& data, const std::filesystem::path& filepath);

	//* execution methods *//

	void SetComplete() { status_ = Status::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const Async::Execution execution_;

	static const inline std::filesystem::path kMetaExtension = ".content";

	Status status_ = Status::None;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* meta helper methods *//

	static std::filesystem::path GetMetaFilepath(const std::filesystem::path& filepath);

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief BaseContentを継承した型のみを受け付けるconcept
template <class T>
concept Content = std::derived_from<T, BaseContent>;

SXAVENGER_ENGINE_NAMESPACE_END
