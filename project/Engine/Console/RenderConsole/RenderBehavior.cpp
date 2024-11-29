#include "RenderBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderBehavior::Init() {
	SetName("render behavior");
	SetToConsole();
}

void RenderBehavior::Term() {
}

void RenderBehavior::SetAttributeImGui() {
	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}
