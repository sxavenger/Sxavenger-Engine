#include "BaseContent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/Runtime/Thread/AsyncThread.h>
#include <Engine/Editors/EditorEngine.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseContent::Execute(const AsyncThread* thread) {
	AsyncLoad(thread->GetContext());
}

void BaseContent::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		RuntimeLogger::LogComment("[BaseContent]", "waiting for content to complete loading... filepath: " + filepath_.generic_string());
	}
}

void BaseContent::SetFilepath(const std::filesystem::path& filepath) {
	filepath_ = filepath;
	AsyncTask::SetTag(filepath.generic_string());
}

const std::filesystem::path& BaseContent::GetFilepath() const {
	StreamLogger::AssertA(!filepath_.empty(), "asset filepath is empty.");
	return filepath_;
}

void BaseContent::ShowInspector() {
	ImGui::SeparatorText(filepath_.filename().generic_string().c_str());
	ImGui::Text("status: %s", magic_enum::enum_name(GetStatus()).data());
	ImGui::Separator();
}

void BaseContent::CheckExist() const {
	StreamLogger::AssertA(std::filesystem::exists(GetFilepath()), "File does not exist: " + GetFilepath().generic_string());
}

std::filesystem::path BaseContent::GetContentPath() const {
	std::filesystem::path filepath = GetFilepath();
	filepath += kContentExtension_;
	return filepath;
}

void BaseContent::SelectInspector(BaseAsset* asset) {
	if (auto editor = sEditorEngine->GetEditor<InspectorEditor>()) {
		editor->SetInspector(asset);
	}
}
