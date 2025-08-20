#include "UBaseContent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/System/Runtime/Thread/AsyncThread.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// UBaseContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UBaseContent::Execute(const AsyncThread* thread) {
	AsyncLoad(thread->GetContext());
}

void UBaseContent::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Logger::LogRuntime("[UBaseContent]: waiting for content to complete loading...", "filepath: " + filepath_.generic_string(), SxavengerLogger::Category::Comment);
	}
}

void UBaseContent::SetFilepath(const std::filesystem::path& filepath) {
	filepath_ = filepath;
	AsyncTask::SetTag(filepath.generic_string());
}

const std::filesystem::path& UBaseContent::GetFilepath() const {
	Exception::Assert(!filepath_.empty(), "asset filepath is empty.");
	return filepath_;
}

void UBaseContent::ShowInspector() {
	ImGui::SeparatorText(filepath_.filename().generic_string().c_str());
	ImGui::Text("status: %s", magic_enum::enum_name(GetStatus()).data());
	ImGui::Separator();
}

void UBaseContent::CheckExist() const {
	Exception::Assert(std::filesystem::exists(GetFilepath()), "File does not exist: " + GetFilepath().generic_string());
}

std::filesystem::path UBaseContent::GetContentPath() const {
	std::filesystem::path filepath = GetFilepath();
	filepath += ".ucontent";
	return filepath;
}
