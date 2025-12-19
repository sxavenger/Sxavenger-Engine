#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/Components/Entity/BehaviourAddress.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 静的シーンオブジェクトクラス
class SceneObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SceneObject(BehaviourAddress&& address) noexcept;
	~SceneObject();

	void Inspectable();

	const BehaviourAddress& GetAddress() const { return address_; }

	bool IsDelete() const { return isDelete_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	BehaviourAddress address_ = nullptr;

	bool isDelete_ = false;

};

SXAVENGER_ENGINE_NAMESPACE_END
