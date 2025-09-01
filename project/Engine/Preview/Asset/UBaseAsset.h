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
// UBaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class UBaseAsset
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

	UBaseAsset(const Uuid& id) : id_(id) {}
	UBaseAsset(const std::nullopt_t&) : id_(std::nullopt) {}
	virtual ~UBaseAsset() = default;

	//* asset option *//

	bool HasId() const { return id_.has_value(); }

	const Uuid& GetId() const { return id_.value(); } //!< todo exceptionの追加

	bool IsComplete() const { return status_ == Status::Complete; }

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

	void WaitComplete() const;

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
concept UAssetConcept = std::derived_from<T, UBaseAsset> && !std::is_same_v<T, UBaseAsset>;
