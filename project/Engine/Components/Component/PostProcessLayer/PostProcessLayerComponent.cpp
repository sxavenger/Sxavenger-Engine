#include "PostProcessLayerComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Graphics/Graphics.h>

//* lib
#include <Lib/Geometry/VectorComparision.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLayerComponent::ShowComponentInspector() {

	if (ImGui::BeginCombo("tag", magic_enum::enum_name(GetTag()).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<Tag>()) {
			if (ImGui::Selectable(name.data(), GetTag() == value)) {
				SetTag(value);
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::TreeNodeEx("process layer", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {

		// swapに関する設定
		std::optional<std::pair<bool, Iterator>> swapData = std::nullopt;
		// nullopt:    swapしない
		// itr, true:  begin側にswap
		// itr, false: end側にswap

		for (auto itr = processes_.begin(); itr != processes_.end(); ++itr) {
			auto process = (*itr).get();

			ImGui::PushID(static_cast<void*>(process));

			// Swap button
			bool isEnableUp = (itr != processes_.begin());
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
	}

	// todo: mouseでの操作を可能にする
	bool isVolume = (tag_ == Tag::Volume);

	if (ImGui::TreeNodeEx("volume", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {

		ImGui::BeginDisabled(!isVolume);

		SxImGui::DragFloat("blend radius", &blendRadius, 0.01f, 0.0f, std::nullopt);
		SxImGui::DragFloat("blend weight", &blendWeight, 0.01f, 0.0f, 1.0f);

		ImGui::EndDisabled();
	}
	
	auto transform = GetTransform();

	if (isVolume && transform != nullptr) {
		//!< volume時, boxの描画
		Vector3f min = Matrix4x4::Transform({ -0.5f, -0.5f, -0.5f }, transform->GetMatrix());
		Vector3f max = Matrix4x4::Transform({ 0.5f, 0.5f, 0.5f }, transform->GetMatrix());

		Graphics::PushBox(min, max, Color4f(0.92f, 0.63f, 0.08f, 1.0f), 0.6f);
	}
}

void PostProcessLayerComponent::Process(const DirectXQueueContext* context, const BasePostProcess::ProcessInfo& info) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, info);
		}
	}
}

const TransformComponent* PostProcessLayerComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->GetComponent<TransformComponent>();
}

float PostProcessLayerComponent::CalculateVolumeWeight(const Vector3f& position) const {

	auto volume = GetVolumeBox();

	if (!volume.has_value()) {
		return 0.0f; //!< volume boxが存在しない.
	}

	const auto& [min, max] = volume.value();

	if (All(position >= min) && All(position <= max)) { //!< volume内にある場合
		return 1.0f;
	}

	if (blendRadius <= 0.0f || blendWeight <= 0.0f) {
		// parameterが無効値.
		return 0.0f;
	}

	Vector3f closest = Vector3f::Clamp(position, min, max);

	float distance = (position - closest).Length();
	float t        = 1.0f - std::clamp(distance / blendRadius, 0.0f, 1.0f);

	return std::clamp(t * blendWeight, 0.0f, 1.0f);
}

std::optional<std::pair<Vector3f, Vector3f>> PostProcessLayerComponent::GetVolumeBox() const {
	auto transform = GetTransform();

	if (transform == nullptr) {
		return std::nullopt;
	}

	// boundingの計算と調整
	Vector3f min = Matrix4x4::Transform({ -0.5f, -0.5f, -0.5f }, transform->GetMatrix());
	Vector3f max = Matrix4x4::Transform({ 0.5f, 0.5f, 0.5f }, transform->GetMatrix());

	for (size_t i = 0; i < 3; ++i) {
		if (min[i] > max[i]) {
			std::swap(min[i], max[i]);
		}
	}

	return std::make_pair(min, max);
}

bool PostProcessLayerComponent::IsInsideVolume(const Vector3f& position) const {

	auto transform = GetTransform();

	if (transform == nullptr) {
		return false;
	}

	// boundingの計算と調整
	Vector3f min = Matrix4x4::Transform({ -0.5f, -0.5f, -0.5f }, transform->GetMatrix());
	Vector3f max = Matrix4x4::Transform({ 0.5f, 0.5f, 0.5f }, transform->GetMatrix());

	for (size_t i = 0; i < 3; ++i) {
		if (min[i] > max[i]) {
			std::swap(min[i], max[i]);
		}
	}

	return All(position >= min) && All(position <= max);
}
