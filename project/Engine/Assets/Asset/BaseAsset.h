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
class BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
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

	bool HasId() const { return id_.has_value(); }

	const Uuid& GetId() const;

	std::string SerializeId() const;

	//* status option *//

	bool IsComplete() const { return status_ == Status::Complete; }

	void WaitComplete() const;

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void SetComplete() { status_ = Status::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const std::optional<Uuid> id_; //!< Assetに紐図いているuuid.

	Status status_ = Status::None; //!< Assetの状態.

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept Asset = std::derived_from<T, BaseAsset>;

SXAVENGER_ENGINE_NAMESPACE_END
