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
class EntityBehaviourStorage {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term();

	//* behaviour option *//

	BehaviourAddress RegisterBehaviour();

	void PushUnregisterQueue(BehaviourAddress& address);
	void PushUnregisterQueue(uintptr_t address); //!< Warning: 内部でのみ使用する.

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

	std::unordered_map<uintptr_t, std::unique_ptr<EntityBehaviour>> behaviours_;

	std::queue<uintptr_t> unregister_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance
////////////////////////////////////////////////////////////////////////////////////////////
static EntityBehaviourStorage* sEntityBehaviourStorage = EntityBehaviourStorage::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END
