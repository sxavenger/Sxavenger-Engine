#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "BehaviourAddress.h"
#include "EntityBehaviour.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviourStorage class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 生成された全EntityBehaviourを一元管理するストレージ
class EntityBehaviourStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<uintptr_t, std::unique_ptr<EntityBehaviour>>;
	//!< (EntityBehaviour*)AddressからEntityBehaviourを管理するstorage. 

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term();

	//* behaviour option *//

	BehaviourAddress RegisterBehaviour();

	void PushUnregisterQueue(BehaviourAddress& address);

	void UnregisterBehaviour();

	//* getter *//

	EntityBehaviour* GetBehaviour(const BehaviourAddress& address) const;

	//* for each *//

	void ForEach(const std::function<void(EntityBehaviour*)>& function) const;

	void ForEachRoot(const std::function<void(EntityBehaviour*)>& function) const;

	void ForEachRootOnly(const std::function<void(EntityBehaviour*)>& function) const;

	//* static behaviour option *//

	void ClearStaticBehaviours();

	void InputJson(const json& data);

	json ParseToJson() const;

	//* singleton *//

	static EntityBehaviourStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	std::queue<uintptr_t> unregister_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance
////////////////////////////////////////////////////////////////////////////////////////////
static EntityBehaviourStorage* sEntityBehaviourStorage = EntityBehaviourStorage::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END
