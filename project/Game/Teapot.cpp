#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {
	SetName("teapot");
	ModelBehavior::model_ = SxavengerGame::LoadModel("resources/model/CG2", "teapot.obj");
}

void Teapot::SetAttributeImGui() {
	ImGui::Text("[SetAttributeImGui]:: Test");
	static float a = 1.0f;
	static float b = 1.0f;
	ImGui::SliderFloat("a", &a, 0.0f, 2.0f);
	ImGui::SliderFloat("b", &b, 0.0f, 2.0f);
}
