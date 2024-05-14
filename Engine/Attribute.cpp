#include "Attribute.h"

#include "Console.h"
#include "ImGuiManager.h"

void Attribute::SetAttributeImGui() {
	ImGui::Text("Not Set Attribute Parameter");
}

void Attribute::SetThisAttribute(const std::string& name) {
	name_ = name;
	Console::GetInstance()->SetAttribute(this);
}
