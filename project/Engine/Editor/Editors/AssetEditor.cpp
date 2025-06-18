#include "AssetEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void AssetEditor::Init() {

}

void AssetEditor::ShowMainMenu() {
}

void AssetEditor::ShowWindow() {
	ShowAssetWindow();
}

void AssetEditor::ShowAssetWindow() {

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Asset ## Engine Asset Editor", nullptr, BaseEditor::GetWindowFlag());

	// todo: layoutの変更
	for (const auto& [type, map] : sAssetStorage->GetStorage()) {
		if (ImGui::TreeNode(type->name())) {

			for (const auto& [filepath, asset] : map) {
				if (ImGui::Selectable(filepath.generic_string().c_str(), CheckSelected(asset.get()))) {
					SetSelected(asset.get());
				}

				// drag and dropの定義
				if (ImGui::BeginDragDropSource()) {
					// payloadを設定
					ImGui::SetDragDropPayload(type->name(), filepath.generic_string().c_str(), filepath.generic_string().size() + 1);

					// ドラッグ中の表示
					ImGui::Text(type->name());
					ImGui::Text(filepath.generic_string().c_str());

					ImGui::EndDragDropSource();
				}

			}

			ImGui::TreePop();
		}
	}

	ImGui::End();

}

bool AssetEditor::CheckSelected(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(asset);
	}

	return false;
}

void AssetEditor::SetSelected(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(asset);
	}
}
