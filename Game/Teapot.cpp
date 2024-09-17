#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {

	model_ = std::make_unique<Model>("resources/model/CG2", "teapot.obj");
	ia_ = model_.get();

	SetMonoBehaviorToConsole("teapot");
}

void Teapot::SetMonoBehaviorImGui() {
	ImGui::Text("[SetMonoBehaviorImGui]:: Test");
}
