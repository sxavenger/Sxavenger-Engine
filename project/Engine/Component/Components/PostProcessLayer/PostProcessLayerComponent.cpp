#include "PostProcessLayerComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLayerComponent::ShowComponentInspector() {

	ImGui::Text("tag");
	ImGui::Separator();

	if (ImGui::BeginCombo("## tag", magic_enum::enum_name(GetTag()).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<Tag>()) {
			if (ImGui::Selectable(name.data(), GetTag() == value)) {
				SetTag(value);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Text("post process layer");
	ImGui::Separator();

	// swapに関する設定
	std::optional<std::pair<bool, Iterator>> swapData = std::nullopt;
	// nullopt:    swapしない
	// itr, true:  begin側にswap
	// itr, false: end側にswap

	for (auto itr = processes_.begin(); itr != processes_.end(); ++itr) {
		auto process = (*itr).get();

		ImGui::PushID(static_cast<void*>(process));

		// Swap button
		bool isEnableUp   = (itr != processes_.begin());
		bool isEnableDown = (itr != std::prev(processes_.end()));

		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0.0f);

		ImGui::BeginDisabled(!isEnableUp);
		if (ImGui::ArrowButton("## up", ImGuiDir_Up)) {
			swapData = std::make_pair(true, itr);
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(!isEnableDown);
		if (ImGui::ArrowButton("## down", ImGuiDir_Down)) {
			swapData = std::make_pair(false, itr);
		}
		ImGui::EndDisabled();

		ImGui::PopStyleVar();

		ImGui::SameLine();

		// Enable checkbox
		ImGui::Checkbox("## isEnabled", &process->IsEnabled());

		ImGui::SameLine();

		if (ImGui::CollapsingHeader(process->GetName().c_str())) {
			process->ShowInspectorImGui();
		}

		ImGui::PopID();
	}

	if (swapData.has_value()) {
		auto& [isUp, itr] = swapData.value();

		if (isUp) {
			if (itr != processes_.begin()) {
				std::iter_swap(itr, std::prev(itr));
			}
		} else {
			if (std::next(itr) != processes_.end()) {
				std::iter_swap(itr, std::next(itr));
			}
		}
	}

	// todo: mouseでの操作を可能にする
}

void PostProcessLayerComponent::Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, buffer, camera);
		}
	}
}
