#include "ComponentStorage.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComponentStorage::UnregisterComponent(const std::type_info* type, const ComponentIterator& iterator) {
	storage_.at(type).erase(iterator);
}

ComponentStorage::ComponentIterator ComponentStorage::RegisterComponent(const std::string& component, EntityBehaviour* behavior) {
	StreamLogger::AssertA(factory_.contains(component), "component factory not registered.");
	const auto& [type, function] = factory_.at(component);
	return storage_[type].emplace(storage_[type].end(), std::move(function(behavior)));
}

const std::type_info* ComponentStorage::GetComponentInfo(const std::string& component) const {
	StreamLogger::AssertA(factory_.contains(component), "component factory not registered.");
	return factory_.at(component).first;
}

ComponentStorage* ComponentStorage::GetInstance() {
	static ComponentStorage instance;
	return &instance;
}
