#include "LightCommon.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InlineShadow structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightCommon::InlineShadow::Init() {
	strength = 0.5f;
	flag     = D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
}

void LightCommon::InlineShadow::ShowInspector() {
	if (ImGui::TreeNodeEx("shadow", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		ImGui::SliderFloat("strength",         &strength, 0.0f, 1.0f);
		ImGui::CheckboxFlags("cull back face", &flag.Get(), D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES);
	}
}
