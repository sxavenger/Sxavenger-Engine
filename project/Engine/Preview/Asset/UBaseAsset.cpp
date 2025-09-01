#include "UBaseAsset.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// UBaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UBaseAsset::ShowInspector() {
	ImGui::SeparatorText(GetStr().c_str());
	ImGui::Text("status: %s", magic_enum::enum_name(status_).data());
	ImGui::Separator();
}

void UBaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< 完了を待つ
	}
}

std::string UBaseAsset::GetStr() const {
	if (!id_.has_value()) {
		return "null";
	}

	return (*id_).Serialize();
}
