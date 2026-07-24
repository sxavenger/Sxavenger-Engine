#include "MonoScriptComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoScriptComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoScriptComponent::MonoScriptComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {
	
}

MonoScriptComponent::~MonoScriptComponent() {
	Reset();
}

void MonoScriptComponent::ShowComponentInspector() {

}

void MonoScriptComponent::Create(const std::string& _namespace, const std::string& _class) {
	instance_ = System::CreateMonoInstance(_namespace, _class);
	// TODO: instanceが再代入(move)される場合, Mono::Instance側での開放をする.
}

void MonoScriptComponent::Reset() {
	instance_.Reset();
}

void MonoScriptComponent::Update() {
}
