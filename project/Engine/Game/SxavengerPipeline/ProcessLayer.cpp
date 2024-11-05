#include "ProcessLayer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseProcessLayer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseProcessLayer::Init() {
}

void BaseProcessLayer::Term() {
}

void BaseProcessLayer::SetToConsole() {
	//sSystemConsole->SetLayer(this);
}

void BaseProcessLayer::SetToConsole(const std::string& name) {
	name_ = name;
	SetToConsole();
}

void BaseProcessLayer::SetLayerAttributeImGui() {
	
}
