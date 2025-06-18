#include "BaseAsset.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/Runtime/Thread/AsyncThread.h>

//* external
#include <imgui.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAsset::Execute(const AsyncThread* thread) {
	Load(thread->GetContext());
}

void BaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Logger::LogRuntime("waiting for asset to complete loading...", "filepath: " + filepath_.generic_string(), SxavengerLogger::Category::Comment);
	}
}

void BaseAsset::SetFilepath(const std::filesystem::path& filepath) {
	filepath_ = filepath;
	SetTag(filepath.generic_string());
}

void BaseAsset::ShowInspector() {
	ImGui::SeparatorText(filepath_.stem().generic_string().c_str());
	ImGui::Separator();
}
