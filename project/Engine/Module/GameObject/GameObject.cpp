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

GameObject::GameObject(const std::string& name) {
	address_ = BehaviourHelper::Create(name);
	address_->SetInspectable([this]() { Inspectable(); });
}

GameObject::~GameObject() {
	BehaviourHelper::Destroy(address_);
}
