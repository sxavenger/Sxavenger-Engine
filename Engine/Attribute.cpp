#include "Attribute.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Console.h"
#include "ImGuiManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Attribute base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Attribute::Term() {
	nodes_.clear();
	console->CheckEraseAttribute(this);
}

void Attribute::SetAttributeImGui() {
	ImGui::Text("Not Set Attribute Parameter");
}

void Attribute::SetAttributeNode(Attribute* other) {
	if (other == this) { //!< 自分自身は入れられない
		return;
	}

	nodes_.insert(other);
}

void Attribute::EraseAttributeNode(Attribute* other) {
	nodes_.erase(other);
	console->CheckEraseAttribute(other);
}

void Attribute::SetThisAttribute(const std::string& name) {
	name_ = name;
	console->SetAttribute(this);
}
