#include "ContentStorage.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentStorage::Emplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content) {
	content->AttachUuid();

	System::PushTask(content->GetAsyncExecution(), content);

	storage_[type][content->GetFilepath()] = content;
}

void ContentStorage::TryEmplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content) {
	if (Contains(type, content->GetFilepath())) {
		RuntimeLogger::LogComment("[UAssetStorage]", "content is already registered in storage. \n filepath: " + content->GetFilepath().generic_string());
		return;
	}

	Emplace(type, content);
}

std::shared_ptr<BaseContent> ContentStorage::GetContent(const std::type_info* type, const std::filesystem::path& filepath) const {
	StreamLogger::AssertA(Contains(type, filepath), "content is not registered in storage.");
	return storage_.at(type).At(filepath);
}

const std::type_info* ContentStorage::GetType(const std::filesystem::path& filepath) const {
	for (const auto& [type, contents] : storage_) {
		if (contents.Contains(filepath)) {
			return type;
		}
	}
	// TODO: registryを使いO(n)を避ける

	return nullptr;
}

void ContentStorage::DragAndDropSource(const std::type_info* type, const std::filesystem::path& filepath) {
	if (ImGui::BeginDragDropSource()) {

		std::string name   = type->name();
		std::string substr = name.substr(name.find_last_of(':') + 1);

		// payloadを設定
		ImGui::SetDragDropPayload(substr.c_str(), filepath.generic_string().c_str(), filepath.generic_string().size() + 1);

		// ドラッグ中の表示
		ImGui::Text(type->name());
		ImGui::Text(filepath.generic_string().c_str());

		ImGui::EndDragDropSource();
	}
}

std::optional<std::filesystem::path> ContentStorage::DragAndDropTargetFilepath(const std::type_info* type) {

	std::optional<std::filesystem::path> result = std::nullopt;

	if (ImGui::BeginDragDropTarget()) {

		std::string name   = type->name();
		std::string substr = name.substr(name.find_last_of(':') + 1);

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(substr.c_str())) {
			result = std::filesystem::path(static_cast<const char*>(payload->Data));
		}

		ImGui::EndDragDropTarget();
	}

	return result;
}

ContentStorage* ContentStorage::GetInstance() {
	static ContentStorage instance;
	return &instance;
}

bool ContentStorage::Contains(const std::type_info* type, const std::filesystem::path& filepath) const {
	return storage_.contains(type) && storage_.at(type).Contains(filepath);
}
