#include "MonoBehaviourStorage.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MonoBehaviourStorage::Term() {
	ClearStaticBehaviours();
	UnregisterBehaviour();

	if (!behaviours_.empty()) {
		StreamLogger::EngineLog("[MonoBehaviourStorage] warning | terminating with registered behaviours.");
	}

	behaviours_.clear();
	StreamLogger::EngineLog("[MonoBehaviourStorage] term.");
}

BehaviourAddress MonoBehaviourStorage::RegisterBehaviour() {
	std::unique_ptr<MonoBehaviour> behaviour = std::make_unique<MonoBehaviour>();
	uintptr_t address = behaviour->GetAddress();

	behaviours_.emplace(address, std::move(behaviour));

	StreamLogger::EngineLog(std::format("[MonoBehaviourStorage] registered behaviour. address: 0x{:x}", address));
	return BehaviourAddress{ address };
}

void MonoBehaviourStorage::PushUnregisterQueue(BehaviourAddress& address) {
	if (address == nullptr) {
		StreamLogger::EngineLog("[MonoBehaviourStorage] warning | unregister behaviour address is nullptr.");
		return;
	}

	if (address == BehaviourAddress::Ownership::Borrowed) {
		StreamLogger::Exception("cannot unregister borrowed behaviour address.");
		return;
	}

	uintptr_t value = address.GetAddress();
	StreamLogger::AssertA(behaviours_.contains(value), std::format("behaviour address not found. address: 0x{:x}", value));
	unregister_.emplace(value);

	address = nullptr;
	StreamLogger::EngineLog(std::format("[MonoBehaviourStorage] pushed behaviour unregister queue. address: 0x{:x}", value));
}

void MonoBehaviourStorage::PushUnregisterQueue(uintptr_t address) {
	StreamLogger::AssertA(behaviours_.contains(address), std::format("behaviour address not found. address: 0x{:x}", address));

	unregister_.emplace(address);
	StreamLogger::EngineLog(std::format("[MonoBehaviourStorage] pushed behaviour unregister queue. (address emplace) address: 0x{:x}", address));
}

void MonoBehaviourStorage::UnregisterBehaviour() {
	while (!unregister_.empty()) {
		uintptr_t address = unregister_.front();
		unregister_.pop();

		StreamLogger::AssertA(behaviours_.contains(address), std::format("behaviour address not found. address: 0x{:x}", address));
		behaviours_.erase(address);

		StreamLogger::EngineLog(std::format("[MonoBehaviourStorage] unregistered behaviour. address: 0x{:x}", address));
	}
}

MonoBehaviour* MonoBehaviourStorage::GetBehaviour(const BehaviourAddress& address) const {
	if (address == nullptr) {
		RuntimeLogger::LogWarning("[MonoBehaviourStorage]", "behaviour address is nullptr.");
	}

	uintptr_t value = address.GetAddress();

	StreamLogger::AssertA(behaviours_.contains(value), std::format("behaviour address not found. address: 0x{:x}", value));
	return behaviours_.at(value).get();
}

void MonoBehaviourStorage::ForEachRoot(const std::function<void(MonoBehaviour*)>& function) const {
	for (const auto& root : behaviours_ | std::views::values | std::views::filter([](const std::unique_ptr<MonoBehaviour>& behaviour) { return !behaviour->HasParent(); })) {
		function(root.get());
		root->ForEachChild(function);
	}
}

void MonoBehaviourStorage::ForEachRootOnly(const std::function<void(MonoBehaviour*)>& function) const {
	for (const auto& root : behaviours_ | std::views::values | std::views::filter([](const std::unique_ptr<MonoBehaviour>& behaviour) { return !behaviour->HasParent(); })) {
		function(root.get());
	}
}

void MonoBehaviourStorage::ClearStaticBehaviours() {
	for (const auto& [address, behaviour] : behaviours_) {
		if (behaviour->GetMobility() == MonoBehaviour::Mobility::Static) {
			unregister_.emplace(address);
		}
	}
}

void MonoBehaviourStorage::InputJson(const json& data) {
	for (const auto& behaviourData : data) {
		BehaviourAddress address = RegisterBehaviour();
		address->InputJson(behaviourData);
		address->SetMobility(MonoBehaviour::Mobility::Static);
	}
}

json MonoBehaviourStorage::PerseToJson() const {
	json root = json::array();

	for (const auto& [address, behaviour] : behaviours_) {
		if (behaviour->GetMobility() == MonoBehaviour::Mobility::Static) {
			root.emplace_back(behaviour->PerseToJson());
		}
	}

	return root;
}

MonoBehaviourStorage* MonoBehaviourStorage::GetInstance() {
	static MonoBehaviourStorage instance;
	return &instance;
}
