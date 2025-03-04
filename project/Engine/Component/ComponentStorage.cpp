#include "ComponentStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComponentStorage::UnregisterComponent(std::type_index type, const ComponentIterator& iterator) {
	storage_.at(type).erase(iterator);
}

ComponentStorage* ComponentStorage::GetInstance() {
	static ComponentStorage instance;
	return &instance;
}
