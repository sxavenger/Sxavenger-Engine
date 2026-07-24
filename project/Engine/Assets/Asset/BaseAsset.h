#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <optional>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 全Assetの基底クラス. uuid/名前/読み込み完了状態を共通で管理する
class BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Assetの読み込み状態
	enum class Status : bool {
		None,     //!< 初期状態
		Complete, //!< 使用可能な状態
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseAsset(const Uuid& id) : id_(id) {}
	BaseAsset(std::nullopt_t) : id_(std::nullopt) {}

	virtual ~BaseAsset() = default;

	//* asset option *//

	//! @brief uuidが割り当てられているかを返す
	//! @retval true  uuidを持つ
	//! @retval false uuidを持たない
	bool HasId() const { return id_.has_value(); }

	//! @brief 割り当てられたuuidを取得する
	//! @return Assetのuuid
	const Uuid& GetId() const;

	//! @brief uuidを文字列へシリアライズする
	//! @return uuidの文字列表現
	std::string SerializeId() const;

	//! @brief Assetの名前を取得する
	//! @return Asset名
	const std::string& GetName() const { return name_; }

	//! @brief Assetの名前を設定する
	//! @param[in] name 設定する名前
	void SetName(const std::string& name) { name_ = name; }

	//* status option *//

	//! @brief 読み込みが完了しているかを返す
	//! @retval true  使用可能(完了済み)
	//! @retval false 未完了
	bool IsComplete() const { return status_ == Status::Complete; }

	//! @brief 読み込みが完了するまで呼び出しスレッドをブロックする
	void WaitComplete() const;

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//! @brief 読み込み完了状態へ遷移させる (派生クラスのSetup完了時に呼ぶ)
	void SetComplete() { status_ = Status::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const std::optional<Uuid> id_; //!< Assetに紐図いているuuid.
	std::string name_ = "Asset";   //!< Assetの名前.

	Status status_ = Status::None; //!< Assetの状態.

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief BaseAssetを継承した型のみを受け付けるconcept
template <class T>
concept Asset = std::derived_from<T, BaseAsset>;

SXAVENGER_ENGINE_NAMESPACE_END
