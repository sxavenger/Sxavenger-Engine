#include "SceneObject.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Components/Entity/BehaviourHelper.h>
#include <Engine/Components/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

SceneObject::SceneObject(BehaviourAddress&& address) noexcept
	: address_(std::move(address)) {

	MonoBehaviour* behaviour = address_.Get();
	behaviour->SetInspectable([this]() { this->Inspectable(); });
}

void SceneObject::Inspectable() {
	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

SceneObject::~SceneObject() {
	BehaviourHelper::Destroy(address_);
}
