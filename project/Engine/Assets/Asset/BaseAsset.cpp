#include "BaseAsset.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAsset::ShowInspector() {
	ImGui::SeparatorText(GetStr().c_str());
	ImGui::Text("status: %s", magic_enum::enum_name(status_).data());
	ImGui::Separator();
}

void BaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< 完了を待つ
	}
}

std::string BaseAsset::GetStr() const {
	if (!id_.has_value()) {
		return "null";
	}

	return (*id_).Serialize();
}
