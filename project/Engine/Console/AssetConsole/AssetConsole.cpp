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

	SxavengerAsset::GetAsset()->SystemDebugGui();

	ImGui::End();
}
