#include "AssetConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

//* engine
#include <Engine/Asset/SxavengerAsset.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetConsole::Init(Console* console) {
	console_ = console;
}

void AssetConsole::Term() {
}

void AssetConsole::UpdateConsole() {
	if (isDisplayAssetConsole_) {
		DisplayAsset();
	}
}

void AssetConsole::DisplayAsset() {
	console_->DockingConsole();
	ImGui::Begin("Asset ## Asset Console", nullptr, console_->GetWindowFlag());

	{ //!< asset tree
		ImGuiWindowFlags flag = ImGuiWindowFlags_AlwaysVerticalScrollbar;
		ImVec2 region         = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("asset tree", { region.x * 0.2f, region.y }, ImGuiChildFlags_Borders, flag);
		SxavengerAsset::GetCollection()->ShowAssetTree();
		ImGui::EndChild();
	}

	ImGui::End();
}

void AssetConsole::ShowAssetMenu() {
	if (ImGui::BeginMenu("Thread")) {
		MenuDummy();
		SxavengerAsset::GetCollection()->SystemDebugGui();
		ImGui::EndMenu();
	}

}

void AssetConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}
