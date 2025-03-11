#include "ComponentStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComponentStorage::UnregisterComponent(const std::type_info* type, const ComponentIterator& iterator) {
	storage_.at(type).erase(iterator);
}

ComponentStorage* ComponentStorage::GetInstance() {
	static ComponentStorage instance;
	return &instance;
}
