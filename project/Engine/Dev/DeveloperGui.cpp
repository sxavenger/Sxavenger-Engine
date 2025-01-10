#include "DeveloperGui.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DeveloperGui methods
////////////////////////////////////////////////////////////////////////////////////////////

void DeveloperGui::Init() {
}

void DeveloperGui::Term() {
}

void DeveloperGui::RenderMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("system")) {
		DummyMenu();
		ShowSystemMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("asset")) {
		DummyMenu();
		ShowAssetMenu();
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

DeveloperGui* DeveloperGui::GetInstance() {
	static DeveloperGui instance;
	return &instance;
}

void DeveloperGui::DummyMenu() {
	ImGui::Dummy({ 200, 0 });
}

void DeveloperGui::ShowSystemMenu() {
	if (ImGui::BeginMenu("window collection")) {
		DummyMenu();

		auto windowCollection = SxavengerSystem::GetGameWindowCollection();
		windowCollection->SystemDebugGui();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("descriptorHeaps")) {
		DummyMenu();

		auto desciptorHeaps = SxavengerSystem::GetDxDescriptorHeaps();
		desciptorHeaps->SystemDebugGui();

		ImGui::EndMenu();
	}
}

void DeveloperGui::ShowAssetMenu() {
	if (ImGui::BeginMenu("thread")) {
		DummyMenu();
		SxavengerAsset::GetCollection()->SystemDebugGui();
		ImGui::EndMenu();
	}
}


