#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/Editors/InspectorEditor.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <concepts>
#include <optional>
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseAsset
	: public BaseInspector {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		None,     //!< 初期状態
		Complete, //!< Setupが完了した状態
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseAsset(const Uuid& id) : id_(id) {}
	BaseAsset(const std::nullopt_t&) : id_(std::nullopt) {}
	virtual ~BaseAsset() = default;

	//* asset option *//

	bool HasId() const { return id_.has_value(); }

	const Uuid& GetId() const { return id_.value(); } //!< todo exceptionの追加

	bool IsComplete() const { return status_ == Status::Complete; }

	void WaitComplete() const;

	//* inspector *//

	virtual void ShowInspector() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Status status_ = Status::None;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void Complete() { status_ = Status::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const std::optional<Uuid> id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::string GetStr() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept AssetConcept = std::derived_from<T, BaseAsset> && !std::is_same_v<T, BaseAsset>;
