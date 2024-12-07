#include "TransformBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformBehavior::SystemAttributeImGui() {

	if (ImGui::TreeNode("Transform Component")) {
		TransformComponent::SetImGuiCommand();
		ImGui::TreePop();
	}

	/*if (ImGui::TreeNode("Guizmo")) {
		ImGui::TreePop();
	}*/

	sConsole->Manipulate(
		ImGuizmo::TRANSLATE,
		ImGuizmo::WORLD,
		this
	);
}
