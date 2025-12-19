#include "GameObject.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Components/Entity/BehaviourHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

GameObject::GameObject() {
	address_ = BehaviourHelper::Create("GameObject");
}

GameObject::GameObject(const std::string& name) {
	address_ = BehaviourHelper::Create(name);
}

GameObject::~GameObject() {
	BehaviourHelper::Destroy(address_);
}
