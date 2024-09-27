#include "Teapot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Teapot::Init() {

	model_ = std::make_unique<Model>("resources/model/CG2", "teapot.obj");
	ModelBehavior::model_ = model_.get();

	SetToConsole("teapot");
}

void Teapot::SetAttributeImGui() {
	ImGui::Text("[SetAttributeImGui]:: Test");
}
