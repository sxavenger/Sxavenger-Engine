#include "BaseAsset.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* external
#include <imgui.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		LogRuntime("waiting for asset to complete loading...", "filepath: " + filepath_.generic_string(), SxavengerLogger::Category::Comment);
	}
}

void BaseAsset::ShowInspector() {
	ImGui::SeparatorText(filepath_.stem().generic_string().c_str());
	ImGui::Separator();
}

bool BaseAsset::IsLoaded() const {
	return state_ != State::Unloaded;
}
