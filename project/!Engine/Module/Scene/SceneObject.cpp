#include "SceneObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObject::Inspectable() {
	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}
