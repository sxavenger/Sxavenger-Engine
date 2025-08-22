#include "UContentStorage.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentStorage::Emplace(const std::type_info* type, const std::shared_ptr<UBaseContent>& content) {
	content->AttachUuid();

	SxavengerSystem::PushTask(content->GetAsyncExecution(), content);

	storage_[type][content->GetFilepath()] = content;
}

void UContentStorage::TryEmplace(const std::type_info* type, const std::shared_ptr<UBaseContent>& content) {
	if (Contains(type, content->GetFilepath())) {
		Logger::CommentRuntime("[UAssetStorage]: content is already registered in storage.", content->GetFilepath().generic_string());
		return;
	}

	Emplace(type, content);
}

std::shared_ptr<UBaseContent> UContentStorage::GetContent(const std::type_info* type, const std::filesystem::path& filepath) const {
	if (!Contains(type, filepath)) {
		Exception::Assert(false, "content is not registered in storage.");
	}

	return storage_.at(type).At(filepath);
}

const std::type_info* UContentStorage::GetType(const std::filesystem::path& filepath) const {
	for (const auto& [type, contents] : storage_) {
		if (contents.Contains(filepath)) {
			return type;
		}
	}
	// TODO: registryを使いO(n)を避ける

	return nullptr;
}

void UContentStorage::DragAndDropSource(const std::type_info* type, const std::filesystem::path& filepath) {
	if (ImGui::BeginDragDropSource()) {
		// payloadを設定
		ImGui::SetDragDropPayload(type->name(), filepath.generic_string().c_str(), filepath.generic_string().size() + 1);

		// ドラッグ中の表示
		ImGui::Text(type->name());
		ImGui::Text(filepath.generic_string().c_str());

		ImGui::EndDragDropSource();
	}
}

std::optional<std::filesystem::path> UContentStorage::DragAndDropTargetFilepath(const std::type_info* type) {
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type->name())) {
			return std::filesystem::path(static_cast<const char*>(payload->Data));
		}

		ImGui::EndDragDropTarget();
	}

	return std::nullopt;
}

UContentStorage* UContentStorage::GetInstance() {
	static UContentStorage instance;
	return &instance;
}

bool UContentStorage::Contains(const std::type_info* type, const std::filesystem::path& filepath) const {
	return storage_.contains(type) && storage_.at(type).Contains(filepath);
}
