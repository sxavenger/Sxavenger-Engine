#include "EntityBehaviourStorage.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviourStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EntityBehaviourStorage::Term() {
	ClearStaticBehaviours();
	UnregisterBehaviour();

	if (!behaviours_.empty()) {
		StreamLogger::EngineLog("[EntityBehaviourStorage] warning | terminating with registered behaviours.");
	}

	behaviours_.clear();
	StreamLogger::EngineLog("[EntityBehaviourStorage] term.");
}

BehaviourAddress EntityBehaviourStorage::RegisterBehaviour() {
	std::unique_ptr<EntityBehaviour> behaviour = std::make_unique<EntityBehaviour>();
	uintptr_t address = behaviour->GetAddress();

	behaviours_.emplace(address, std::move(behaviour));

	StreamLogger::EngineLog(std::format("[EntityBehaviourStorage] registered behaviour. address: 0x{:x}", address));
	return BehaviourAddress{ address };
}

void EntityBehaviourStorage::PushUnregisterQueue(BehaviourAddress& address) {
	if (address == nullptr) {
		StreamLogger::EngineLog("[EntityBehaviourStorage] warning | unregister behaviour address is nullptr.");
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
	StreamLogger::EngineLog(std::format("[EntityBehaviourStorage] pushed behaviour unregister queue. address: 0x{:x}", value));
}

void EntityBehaviourStorage::PushUnregisterQueue(uintptr_t address) {
	StreamLogger::AssertA(behaviours_.contains(address), std::format("behaviour address not found. address: 0x{:x}", address));

	unregister_.emplace(address);
	StreamLogger::EngineLog(std::format("[EntityBehaviourStorage] pushed behaviour unregister queue. (address emplace) address: 0x{:x}", address));
}

void EntityBehaviourStorage::UnregisterBehaviour() {
	while (!unregister_.empty()) {
		uintptr_t address = unregister_.front();
		unregister_.pop();

		StreamLogger::AssertA(behaviours_.contains(address), std::format("behaviour address not found. address: 0x{:x}", address));
		behaviours_.erase(address);

		StreamLogger::EngineLog(std::format("[EntityBehaviourStorage] unregistered behaviour. address: 0x{:x}", address));
	}
}

EntityBehaviour* EntityBehaviourStorage::GetBehaviour(const BehaviourAddress& address) const {
	if (address == nullptr) {
		RuntimeLogger::LogWarning("[EntityBehaviourStorage]", "behaviour address is nullptr.");
	}

	uintptr_t value = address.GetAddress();

	StreamLogger::AssertA(behaviours_.contains(value), std::format("behaviour address not found. address: 0x{:x}", value));
	return behaviours_.at(value).get();
}

void EntityBehaviourStorage::ForEachRoot(const std::function<void(EntityBehaviour*)>& function) const {
	for (const auto& root : behaviours_ | std::views::values | std::views::filter([](const std::unique_ptr<EntityBehaviour>& behaviour) { return !behaviour->HasParent(); })) {
		function(root.get());
		root->ForEachChild(function);
	}
}

void EntityBehaviourStorage::ForEachRootOnly(const std::function<void(EntityBehaviour*)>& function) const {
	for (const auto& root : behaviours_ | std::views::values | std::views::filter([](const std::unique_ptr<EntityBehaviour>& behaviour) { return !behaviour->HasParent(); })) {
		function(root.get());
	}
}

void EntityBehaviourStorage::ClearStaticBehaviours() {
	for (const auto& [address, behaviour] : behaviours_) {
		if (behaviour->GetMobility() == EntityBehaviour::Mobility::Static) {
			unregister_.emplace(address);
		}
	}
}

void EntityBehaviourStorage::InputJson(const json& data) {
	for (const auto& behaviourData : data) {
		BehaviourAddress address = RegisterBehaviour();
		address->InputJson(behaviourData);
		address->SetMobility(EntityBehaviour::Mobility::Static);
	}
}

json EntityBehaviourStorage::ParseToJson() const {
	json root = json::array();

	for (const auto& [address, behaviour] : behaviours_) {
		if (behaviour->GetMobility() == EntityBehaviour::Mobility::Static) {
			root.emplace_back(behaviour->ParseToJson());
		}
	}

	return root;
}

EntityBehaviourStorage* EntityBehaviourStorage::GetInstance() {
	static EntityBehaviourStorage instance;
	return &instance;
}
