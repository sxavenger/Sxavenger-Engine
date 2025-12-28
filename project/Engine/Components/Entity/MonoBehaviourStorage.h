#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "BehaviourAddress.h"
#include "MonoBehaviour.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviourStorage {
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

	MonoBehaviour* GetBehaviour(const BehaviourAddress& address) const;

	//* for each *//

	void ForEachRoot(const std::function<void(MonoBehaviour*)>& function) const;

	void ForEachRootOnly(const std::function<void(MonoBehaviour*)>& function) const;

	//* static behaviour option *//

	void ClearStaticBehaviours();

	void InputJson(const json& data);

	json PerseToJson() const;

	//* singleton *//

	static MonoBehaviourStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<uintptr_t, std::unique_ptr<MonoBehaviour>> behaviours_;

	std::queue<uintptr_t> unregister_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance
////////////////////////////////////////////////////////////////////////////////////////////
static MonoBehaviourStorage* sMonoBehaviourStorage = MonoBehaviourStorage::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END
